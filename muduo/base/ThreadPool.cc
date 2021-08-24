// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "muduo/base/ThreadPool.h"

#include "muduo/base/Exception.h"

#include <assert.h>
#include <stdio.h>

using namespace muduo;

/// 线程池初始化
ThreadPool::ThreadPool(const string& nameArg)
  : mutex_(),
    notEmpty_(mutex_),
    notFull_(mutex_),
    name_(nameArg),
    maxQueueSize_(0),
    running_(false)
{
}

ThreadPool::~ThreadPool()
{
  if (running_)
  {
    stop();
  }
}

/// 线程池start
void ThreadPool::start(int numThreads)
{
  assert(threads_.empty());
  running_ = true;  // running
  // 分配空间, 即capacity的值 
  threads_.reserve(numThreads);
  for (int i = 0; i < numThreads; ++i)
  {
    char id[32];
    snprintf(id, sizeof id, "%d", i+1);

    /// 创建线程初始化执行runInThread函数, 并进入线程池
    threads_.emplace_back(new muduo::Thread(
          std::bind(&ThreadPool::runInThread, this), name_+id));
    /// 该线程开始运行
    threads_[i]->start();
  }

  /// 线程数目为0
  if (numThreads == 0 && threadInitCallback_)
  {
    threadInitCallback_();
  }
}

void ThreadPool::stop()
{
  {
  MutexLockGuard lock(mutex_);
  running_ = false;
  notEmpty_.notifyAll();
  notFull_.notifyAll();
  }

  /// 等待所有线程执行完毕
  for (auto& thr : threads_)
  {
    thr->join();
  }
}

/// 返回线程池的size
size_t ThreadPool::queueSize() const
{
  MutexLockGuard lock(mutex_);
  return queue_.size();
}

void ThreadPool::run(Task task)
{
  /// 如果线程为空, 主线程执行task
  if (threads_.empty())
  {
    task();
  }
  else
  {
    MutexLockGuard lock(mutex_);
    while (isFull() && running_)
    {
      /// 等待notFull_ is true
      notFull_.wait();
    }
    if (!running_) return;
    assert(!isFull());
    /// 任务task入队列
    queue_.push_back(std::move(task));
    /// 发出任务队列非空条件变量
    notEmpty_.notify();
  }
}

ThreadPool::Task ThreadPool::take()
{
  MutexLockGuard lock(mutex_);
  // always use a while-loop, due to spurious wakeup
  while (queue_.empty() && running_)
  {
    /// 等待任务队列非空, notEmpty_ is true
    notEmpty_.wait();
  }
  Task task;
  if (!queue_.empty())
  {
    /// 任务出队列
    task = queue_.front();
    queue_.pop_front();

    if (maxQueueSize_ > 0)
    {
      /// 释放任务队列未满条件变量
      notFull_.notify();
    }
  }
  return task;
}

bool ThreadPool::isFull() const
{
  mutex_.assertLocked();
  /// queue_.size() >= maxQueueSize_说明队列满了
  return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

/// 线程池的线程执行函数
void ThreadPool::runInThread()
{
  try
  {
    if (threadInitCallback_)
    {
      threadInitCallback_();
    }
    // 控制线程的执行
    while (running_)
    {
      /// 任务出队列
      Task task(take());

      /// 执行任务
      if (task)
      {
        task();
      }
    }
  }
  catch (const Exception& ex)
  {
    fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
    abort();
  }
  catch (const std::exception& ex)
  {
    fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    abort();
  }
  catch (...)
  {
    fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
    throw; // rethrow
  }
}

