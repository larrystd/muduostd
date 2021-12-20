// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef MUDUO_NET_INSPECT_INSPECTOR_H
#define MUDUO_NET_INSPECT_INSPECTOR_H

#include "muduo/base/Mutex.h"
#include "http/HttpRequest.h" // 基于http协议
#include "http/HttpServer.h"

#include <map>

namespace muduo
{
namespace net
{

class ProcessInspector; // 三个监视class, 分别是Process, Performance, System
class PerformanceInspector;
class SystemInspector;

// An internal inspector of the running process, usually a singleton.
// Better to run in a seperated thread, as some method may block for seconds
class Inspector : noncopyable
{
 public:
  typedef std::vector<string> ArgList;
  typedef std::function<string (HttpRequest::Method, const ArgList& args)> Callback;
  // 构造通过,loop, address
  Inspector(EventLoop* loop,
            const InetAddress& httpAddr,
            const string& name);
  ~Inspector();

  /// Add a Callback for handling the special uri : /mudule/command
  void add(const string& module,
           const string& command,
           const Callback& cb,
           const string& help);
  void remove(const string& module, const string& command);

 private:
  typedef std::map<string, Callback> CommandList; // 回调函数map
  typedef std::map<string, string> HelpList;

  void start();
  void onRequest(const HttpRequest& req, HttpResponse* resp); 

  HttpServer server_; // 内置HttpServer_对象
  // 用unique_ptr维护process, performance, system监视对象
  std::unique_ptr<ProcessInspector> processInspector_;
  std::unique_ptr<PerformanceInspector> performanceInspector_;
  std::unique_ptr<SystemInspector> systemInspector_;
  MutexLock mutex_;

  std::map<string, CommandList> modules_ GUARDED_BY(mutex_);

  std::map<string, HelpList> helps_ GUARDED_BY(mutex_); // map内容HelpList还是map, 双层map
};

}  // namespace net
}  // namespace muduo

#endif  // MUDUO_NET_INSPECT_INSPECTOR_H
