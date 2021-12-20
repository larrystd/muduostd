// Benchmark inspired by libevent/test/bench.c
// See also: http://libev.schmorp.de/bench.html

#include "muduo/base/Logging.h"
#include "muduo/base/Thread.h"
#include "muduo/net/Channel.h"
#include "muduo/net/EventLoop.h"

#include <stdio.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

std::vector<int> g_pipes; // 维护fd列表
int numPipes;
int numActive;
int numWrites;
EventLoop* g_loop;  // loops
std::vector<std::unique_ptr<Channel>> g_channels; // 维护channel列表

int g_reads, g_writes, g_fired; // 分别表示读写的数据量, 活跃连接数


void readCallback(Timestamp, int fd, int idx) { // 读回调
  char ch;
  g_reads += static_cast<int>(::recv(fd, &ch, sizeof(ch), 0));  // 接收数据
  if (g_writes > 0) { // 还可写
    int widx = idx+1;
    if (widx >= numPipes) {
      widx -= numPipes;
    }

    ::send(g_pipes[2 * widx + 1], "m", 1, 0); // 发送m数据
    g_writes--;
    g_fired++;
  }
  if (g_fired == g_reads) {
    g_loop->quit();
  }
}

std::pair<int, int> runOnce() {
  Timestamp beforeInit(Timestamp::now());
  for (int i = 0; i < numPipes; ++i) {
    Channel& channel = *g_channels[i];  // 获得channel对象
    channel.setReadCallback(std::bind(readCallback, _1, channel.fd(), i));  // 可读回调
    channel.enableReading();  // 注册channel到poll中, 可读事件
  }

  int space = numPipes / numActive;
  space *= 2;

  for (int i = 0; i < numActive; ++i) { // 一次向
    ::send(g_pipes[i * space + 1], "m",1, 0); // 发送信息到fd, g_loop会有可读事件活跃
  }

  g_fired = numActive;
  g_reads = 0;
  g_writes = numWrites; // 可写数量
  Timestamp beforeLoop(Timestamp::now());
  g_loop->loop(); // 执行事件监听, 执行可读回调, 额发送信息和接收都是一个线程

  Timestamp end(Timestamp::now());

  int iterTime = static_cast<int>(end.microSecondsSinceEpoch() - beforeInit.microSecondsSinceEpoch());  // 表示runOnce全部执行时间
  int loopTime = static_cast<int>(end.microSecondsSinceEpoch() - beforeLoop.microSecondsSinceEpoch());  // 执行时间监听和处理需要的时间
  
  return std::make_pair(iterTime, loopTime);
}

int main(int argc, char* argv[])
{
  numPipes = 100;
  numActive = 1;
  numWrites = 100;
  int c;
  while ((c = getopt(argc, argv, "n:a:w:")) != -1)  // 解析参数,  ./pingpong_bench n 100 a 40 w 200
  {
    switch (c)
    {
      case 'n':
        numPipes = atoi(optarg);  // fd数量
        break;
      case 'a':
        numActive = atoi(optarg); // 一次活跃数量
        break;
      case 'w':
        numWrites = atoi(optarg); // 一次可写数量
        break;
      default:
        fprintf(stderr, "Illegal argument \"%c\"\n", c);
        return 1;
    }
  }

  struct rlimit rl;
  rl.rlim_cur = rl.rlim_max = numPipes * 2 + 50;
  if (::setrlimit(RLIMIT_NOFILE, &rl) == -1)
  {
    perror("setrlimit");
    //return 1;  // comment out this line if under valgrind
  }
  g_pipes.resize(2 * numPipes);
  for (int i = 0; i < numPipes; ++i)
  {
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, &g_pipes[i*2]) == -1) // 创建一个fd到g_pipes列表中
    {
      perror("pipe");
      return 1;
    }
  }

  EventLoop loop;
  g_loop = &loop;

  for (int i = 0; i < numPipes; ++i)
  {
    Channel* channel = new Channel(&loop, g_pipes[i*2]);  // 基于fd创建一些channel, 用g_loop维护
    g_channels.emplace_back(channel); // 加入到列表g_channel中
  }

  for (int i = 0; i < 25; ++i)
  {
    std::pair<int, int> t = runOnce();
    printf("%8d %8d\n", t.first, t.second);
  }

  for (const auto& channel : g_channels)
  {
    channel->disableAll();
    channel->remove();
  }
  g_channels.clear();
}

