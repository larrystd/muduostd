#include "inspect/Inspector.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/EventLoopThread.h"

using namespace muduo;
using namespace muduo::net;

int main()
{
  EventLoop loop;
  EventLoopThread t;
  Inspector ins(t.startLoop(), InetAddress(12345), "test"); // Inspector对象, 基于http协议
  loop.loop();
}

