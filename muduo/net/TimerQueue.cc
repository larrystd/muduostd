// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include "muduo/net/TimerQueue.h"

#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/Timer.h"
#include "muduo/net/TimerId.h"

#include <sys/timerfd.h>
#include <unistd.h>

namespace muduo
{
namespace net
{
namespace detail
{

int createTimerfd()
{

  /// 创建timerfd
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                 TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd < 0)
  {
    LOG_SYSFATAL << "Failed in timerfd_create";
  }
  return timerfd;
}


/// 现在到when时间间隔
struct timespec howMuchTimeFromNow(Timestamp when)
{
  int64_t microseconds = when.microSecondsSinceEpoch()
                         - Timestamp::now().microSecondsSinceEpoch();
  if (microseconds < 100)
  {
    microseconds = 100;
  }
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(
      microseconds / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(
      (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
  return ts;
}

/// 读timerfd
void readTimerfd(int timerfd, Timestamp now)
{
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
  LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
  if (n != sizeof howmany)
  {
    LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
  }
}

/// 更新定时器fd设置, 用新的expiration时间戳来设置timerfd
/// timerfd的超时时间是定时集合中最早的时间戳
void resetTimerfd(int timerfd, Timestamp expiration)
{
  // wake up loop by timerfd_settime()
  struct itimerspec newValue;
  struct itimerspec oldValue;
  memZero(&newValue, sizeof newValue);
  memZero(&oldValue, sizeof oldValue);

  newValue.it_value = howMuchTimeFromNow(expiration);
  /// oldValue返回之前的定时器超时时间
  /// interval设置为0
  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
  if (ret)
  {
    LOG_SYSERR << "timerfd_settime()";
  }
}

}  // namespace detail
}  // namespace net
}  // namespace muduo

using namespace muduo;
using namespace muduo::net;
using namespace muduo::net::detail;

/// 初始化TimerQueue
TimerQueue::TimerQueue(EventLoop* loop)
  : loop_(loop),
    timerfd_(createTimerfd()),
    /// 将timerfd_封装成Channel
    timerfdChannel_(loop, timerfd_),
    timers_(),
    callingExpiredTimers_(false)
{
  /// 设置channel 读回调函数为handleRead,
  timerfdChannel_.setReadCallback(
      std::bind(&TimerQueue::handleRead, this));
  // we are always reading the timerfd, we disarm it with timerfd_settime.
  timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
  /// Channel处理
  timerfdChannel_.disableAll();
  timerfdChannel_.remove();
  /// 关闭timerfd_
  ::close(timerfd_);
  // do not remove channel, since we're in EventLoop::dtor();
  for (const Entry& timer : timers_)
  {
    delete timer.second;
  }
}



TimerId TimerQueue::addTimer(TimerCallback cb,
                             Timestamp when,
                             double interval)
{
  /// 将TimerCallback, when封装成timer
  Timer* timer = new Timer(std::move(cb), when, interval);
  /// 在loop线程中执行addTimerInLoop
  loop_->runInLoop(
      std::bind(&TimerQueue::addTimerInLoop, this, timer));
  
  /// 封装成TimerId
  return TimerId(timer, timer->sequence());
}

void TimerQueue::cancel(TimerId timerId)
{
  loop_->runInLoop(
      std::bind(&TimerQueue::cancelInLoop, this, timerId));
}


void TimerQueue::addTimerInLoop(Timer* timer)
{
  loop_->assertInLoopThread();
  /// 将timer插入定时队列, 最早时间是否发生改变
  bool earliestChanged = insert(timer);

  if (earliestChanged)
  {
    /// 需要修改最早时间的定时集合
    /// 最早时间定时集合就是第一个元素, 现在是timer->expiration()了
    resetTimerfd(timerfd_, timer->expiration());
  }
}

/// 从timers_删除TimerId的对象
void TimerQueue::cancelInLoop(TimerId timerId)
{
  loop_->assertInLoopThread();
  assert(timers_.size() == activeTimers_.size());
  /// 根据timerId构造timer对象(其实是std::pair<Timer*, int64_t>)

  ActiveTimer timer(timerId.timer_, timerId.sequence_);
  /// 找到等于timer的对象(即比较Timer指针)
  ActiveTimerSet::iterator it = activeTimers_.find(timer);
  if (it != activeTimers_.end())
  {
    /// 从timer中和activeTimers_擦除key
    /// 删除时间戳为it->first->expiration()的对象
    size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
    assert(n == 1); (void)n;
    delete it->first; // FIXME: no delete please
    activeTimers_.erase(it);
  }
  else if (callingExpiredTimers_)
  {
    cancelingTimers_.insert(timer);
  }
  assert(timers_.size() == activeTimers_.size());
}


/// 当timerfd可读时, 调用之
void TimerQueue::handleRead()
{
  loop_->assertInLoopThread();

  /// 读readTimerfd
  Timestamp now(Timestamp::now());
  readTimerfd(timerfd_, now);

  /// 找到比now时间早的定时序列
  std::vector<Entry> expired = getExpired(now);

  callingExpiredTimers_ = true;
  cancelingTimers_.clear();
  // safe to callback outside critical section
  /// 执行超期定时序列的任务
  for (const Entry& it : expired)
  {
    it.second->run();
  }
  callingExpiredTimers_ = false;

  /// 更新timers和timerfd
  reset(expired, now);
}


/// 超期的定时器
std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
  assert(timers_.size() == activeTimers_.size());

  std::vector<Entry> expired;

  /// 以当前时间戳构造的Entry
  Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
  /// 找到不小于当前时间的第一个元素(的迭代器)
  /// end前面的都是小于当前时间的
  TimerList::iterator end = timers_.lower_bound(sentry);
  assert(end == timers_.end() || now < end->first); // 未越界
  /// 将begin()到end的元素倒序插入到expired中
  std::copy(timers_.begin(), end, back_inserter(expired));
  /// timers_擦除begin()到end的元素
  timers_.erase(timers_.begin(), end);

  for (const Entry& it : expired)
  {
    /// 从activeTimers_擦除expired中的元素
    ActiveTimer timer(it.second, it.second->sequence());
    size_t n = activeTimers_.erase(timer);
    assert(n == 1); (void)n;
  }

  assert(timers_.size() == activeTimers_.size());
  return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
  Timestamp nextExpire;

  /// 对超期的定时对象
  for (const Entry& it : expired)
  {
    ActiveTimer timer(it.second, it.second->sequence());
    /// 如果该对象是要重复的, 即每隔多久执行一次那种
    if (it.second->repeat()
        && cancelingTimers_.find(timer) == cancelingTimers_.end())
    {
      /// timer.restart, 重新加入timer定时集合
      it.second->restart(now);
      insert(it.second);
    }
    else
    {
      // FIXME move to a free list
      delete it.second; // FIXME: no delete please
    }
  }
  /// 重置timefd
  if (!timers_.empty())
  {
    nextExpire = timers_.begin()->second->expiration();
  }

  if (nextExpire.valid())
  {
    resetTimerfd(timerfd_, nextExpire);
  }
}


/// 在定时集合activeTimers_中增加timer
bool TimerQueue::insert(Timer* timer)
{
  loop_->assertInLoopThread();
  assert(timers_.size() == activeTimers_.size());
  bool earliestChanged = false;
  /// timer的超时时间
  Timestamp when = timer->expiration();
  /// activeTimers_的第一个元素
  TimerList::iterator it = timers_.begin();

  /// 如果超时时间小于第一个定时元素的时间戳,（第一个是最早的)
  /// 需要修改最早元素
  if (it == timers_.end() || when < it->first)
  {
    earliestChanged = true;
  }
  {
    /// 时间戳-timer 对加入到定时集合中
    /// 集合是自然有序的
    std::pair<TimerList::iterator, bool> result
      = timers_.insert(Entry(when, timer));
    assert(result.second); (void)result;
  }
  {
    /// timer加入到活跃timer集合
    std::pair<ActiveTimerSet::iterator, bool> result
      = activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
    assert(result.second); (void)result;
  }

  assert(timers_.size() == activeTimers_.size());
  return earliestChanged;
}

