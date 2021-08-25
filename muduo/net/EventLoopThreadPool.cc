// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "muduo/net/EventLoopThreadPool.h"

#include "muduo/net/EventLoop.h"
#include "muduo/net/EventLoopThread.h"

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const string& nameArg)
  : baseLoop_(baseLoop),
  /// 线程名
    name_(nameArg),
    started_(false),
    numThreads_(0),
    next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
  // Don't delete loop, it's stack variable
}

/// 开启线程池
void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
  assert(!started_);
  /// 必须是创建过eventloop的线程
  baseLoop_->assertInLoopThread();
  // 开启线程池
  started_ = true;
  // 创建threads和loops
  for (int i = 0; i < numThreads_; ++i)
  {
    /// buf, 储存线程名
    char buf[name_.size() + 32];
    snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);

    // 构造EventLoopThread对象(同时创建了新线程)
    EventLoopThread* t = new EventLoopThread(cb, buf);
    /// threads 列表
    threads_.push_back(std::unique_ptr<EventLoopThread>(t));
    /// 开启新线程执行loop , 老线程返回loop, 加入列表
    loops_.push_back(t->startLoop());
  }
  /// 不创建新线程， 当前线程执行cb
  if (numThreads_ == 0 && cb)
  {
    cb(baseLoop_);
  }
}

/// loop 是一个vector, 得到下一个loop
EventLoop* EventLoopThreadPool::getNextLoop()
{
  baseLoop_->assertInLoopThread();
  assert(started_);
  EventLoop* loop = baseLoop_;

  if (!loops_.empty())
  {
    // round-robin
    loop = loops_[next_];
    ++next_;
    if (implicit_cast<size_t>(next_) >= loops_.size())
    {
      next_ = 0;
    }
  }
  return loop;
}

EventLoop* EventLoopThreadPool::getLoopForHash(size_t hashCode)
{
  baseLoop_->assertInLoopThread();
  EventLoop* loop = baseLoop_;

  if (!loops_.empty())
  {
    loop = loops_[hashCode % loops_.size()];
  }
  return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
  baseLoop_->assertInLoopThread();
  assert(started_);
  if (loops_.empty())
  {
    return std::vector<EventLoop*>(1, baseLoop_);
  }
  else
  {
    return loops_;
  }
}
