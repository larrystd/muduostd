// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

// 异步日志

#include "muduo/base/AsyncLogging.h"
#include "muduo/base/LogFile.h"
#include "muduo/base/Timestamp.h"

#include <stdio.h>

using namespace muduo;

AsyncLogging::AsyncLogging(const string& basename,
                           off_t rollSize,
                           int flushInterval)
  : flushInterval_(flushInterval),
    running_(false),
    basename_(basename),
    rollSize_(rollSize),
    thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
    latch_(1),
    mutex_(),
    cond_(mutex_),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    buffers_()
{
  /// 重置三个buffer
  currentBuffer_->bzero();
  nextBuffer_->bzero();
  buffers_.reserve(16);
}

/// 在日志中增加记录， logline指针, 大小为len
void AsyncLogging::append(const char* logline, int len)
{
  muduo::MutexLockGuard lock(mutex_);
  /// 剩余空间大于len, 加入之
  if (currentBuffer_->avail() > len)
  {
    currentBuffer_->append(logline, len);
  }
  else
  {
    /// 内容不够, 1. 将currentBuffer_加入到buffers中
    buffers_.push_back(std::move(currentBuffer_));
    /// 2. 更改currentBuffer指针。如果nextBuffer有空间, currentBuffer指向nextBuffer, 没有, 则currentBuffer指向新的buffer对象
    if (nextBuffer_)
    {
      currentBuffer_ = std::move(nextBuffer_);
    }
    else
    {
      currentBuffer_.reset(new Buffer); // Rarely happens
    }
    /// 3. 在currentBuffer加入数据
    currentBuffer_->append(logline, len);
    cond_.notify();
  }
}


void AsyncLogging::threadFunc()
{
  assert(running_ == true);
  latch_.countDown();
  LogFile output(basename_, rollSize_, false);
  BufferPtr newBuffer1(new Buffer);
  BufferPtr newBuffer2(new Buffer);
  newBuffer1->bzero();
  newBuffer2->bzero();
  /// 要写的buffers vector
  BufferVector buffersToWrite;
  buffersToWrite.reserve(16);
  while (running_)
  {
    assert(newBuffer1 && newBuffer1->length() == 0);
    assert(newBuffer2 && newBuffer2->length() == 0);
    assert(buffersToWrite.empty());

    /// 得到要写的日志vector, buffersToWrite
    {
      muduo::MutexLockGuard lock(mutex_);

      /// buffers_为空, 等待buffers_有数据
      if (buffers_.empty())  // unusual usage!
      {
        cond_.waitForSeconds(flushInterval_);
      }
      /// 处理currentBuffer_
      buffers_.push_back(std::move(currentBuffer_));
      currentBuffer_ = std::move(newBuffer1);
      /// 将buffers cas到buffersToWrite
      buffersToWrite.swap(buffers_);
      if (!nextBuffer_)
      {
        nextBuffer_ = std::move(newBuffer2);
      }
    }

    assert(!buffersToWrite.empty());
    // 给日志加时间戳等信息
    // 将时间戳信息写入到file
    if (buffersToWrite.size() > 25)
    {
      char buf[256];
      snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
               Timestamp::now().toFormattedString().c_str(),
               buffersToWrite.size()-2);
      fputs(buf, stderr);

      /// 将buf内容写入到日志中
      output.append(buf, static_cast<int>(strlen(buf)));
      buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
    }
    /// 将buffersToWrite内容写入到file
    for (const auto& buffer : buffersToWrite)
    {
      // FIXME: use unbuffered stdio FILE ? or use ::writev ?
      output.append(buffer->data(), buffer->length());
    }
    /// 如果buffersToWrite的buffer数量大于2 , 重新设置为2
    if (buffersToWrite.size() > 2)
    {
      // drop non-bzero-ed buffers, avoid trashing
      buffersToWrite.resize(2);
    }

    /// 从buffersToWrite拿出内容给newBuffer1和newBuffer2
    if (!newBuffer1)
    {
      assert(!buffersToWrite.empty());
      newBuffer1 = std::move(buffersToWrite.back());
      buffersToWrite.pop_back();
      newBuffer1->reset();
    }

    if (!newBuffer2)
    {
      assert(!buffersToWrite.empty());
      newBuffer2 = std::move(buffersToWrite.back());
      buffersToWrite.pop_back();
      newBuffer2->reset();
    }

    buffersToWrite.clear();
    output.flush();
  }

  /// output文件缓冲区刷新到文件中
  output.flush();
}

