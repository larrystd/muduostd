// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "muduo/net/EventLoopThread.h"

#include "muduo/net/EventLoop.h"

using namespace muduo;
using namespace muduo::net;

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb,
                                 const string& name)
  : loop_(NULL),
    exiting_(false),
    // 绑定线程执行的函数, EventLoopThread::threadFunc
    thread_(std::bind(&EventLoopThread::threadFunc, this), name),
    mutex_(),
    cond_(mutex_),
    callback_(cb)
{
}

EventLoopThread::~EventLoopThread()
{
  exiting_ = true;
  if (loop_ != NULL) // not 100% race-free, eg. threadFunc could be running callback_.
  {
    // still a tiny chance to call destructed object, if threadFunc exits just now.
    // but when EventLoopThread destructs, usually programming is exiting anyway.
    loop_->quit();
    thread_.join();
  }
}

// 开启线程(执行函数)
EventLoop* EventLoopThread::startLoop()
{
  assert(!thread_.started());
  // 开启新线程thread执行start()

  thread_.start();

  EventLoop* loop = NULL;
  {
    MutexLockGuard lock(mutex_);
    /// 老线程等待loop_
    while (loop_ == NULL)
    {
      cond_.wait();
    }
    /// loop有了(新线程创建之)
    loop = loop_;
  }
  /// 
  return loop;
}

// 新线程执行的函数
void EventLoopThread::threadFunc()
{
   /// 在栈上运行的eventloop， 创建eventloop对象
  EventLoop loop;

  if (callback_)
  {
    // 执行回调函数ThreadInitCallback
    callback_(&loop);
  }

  {
    MutexLockGuard lock(mutex_);
    loop_ = &loop;
    /// 唤醒老线程返回loop
    cond_.notify();
  }
  // 执行loop循环
  loop.loop();
  //assert(exiting_);
  MutexLockGuard lock(mutex_);
  loop_ = NULL;
}

