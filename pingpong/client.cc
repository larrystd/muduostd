#include "muduo/net/TcpClient.h"

#include "muduo/base/Logging.h"
#include "muduo/base/Thread.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/EventLoopThreadPool.h"
#include "muduo/net/InetAddress.h"

#include <utility>

#include <stdio.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

class Client; // 客户端

class Session : noncopyable { // Session, 维护TcpClient对象, 并记录一些信息, 例如owner, bytesRead等
 public:
  Session(EventLoop* loop,
        const InetAddress& serverAddr,
        const string& name,
        Client* owner)  // owner是所属的Client对象, 一个Client用多线程connect进行pingpong
  : client_(loop, serverAddr, name),  // 构建client对象
    owner_ (owner),
    bytesRead_(0),
    bytesWritten_(0),
    messagesRead_(0)
  {
    // 回调函数
    client_.setConnectionCallback(
      std::bind(&Session::onConnection, this, _1)
    );

    client_.setMessageCallback(
      std::bind(&Session::onMessage, this, _1, _2, _3)
    ); 
  }

  void start() {
    client_.connect();
  }

  void stop() {
    client_.disconnect();
  }

  int64_t bytesRead() const {
    return bytesRead_;
  }

  int64_t messagesRead() const {
    return messagesRead_;
  }


 private:
  void onConnection(const TcpConnectionPtr& conn);

  void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp timestamp) {
    ++messagesRead_;
    bytesRead_ += buf->readableBytes(); // 统计传输的数据, 这个buffer实际上服务器传过来的(服务器原路返回, 因此这还是onconnect时客户端发的)
    bytesWritten_ += buf->readableBytes();
    conn->send(buf);  // 将Buf的内容发走
  }

  TcpClient client_;
  Client* owner_;
  int64_t bytesRead_;
  int64_t bytesWritten_;
  int64_t messagesRead_;
};

class Client : noncopyable {
 public:
  Client(EventLoop* loop,
        const InetAddress& serverAddr,
        int blockSize,
        int sessionCount,
        int timeout,
        int threadCount)
    : loop_(loop),
      threadPool_(loop, "pingpong-client"),
      sessionCount_(sessionCount),
      timeout_(timeout)
  {
    loop->runAfter(timeout, std::bind(&Client::handleTimeout, this)); // timeout之后运行&Client::handleTimeout, 即终止session
    if (threadCount > 1) {
      threadPool_.setThreadNum(threadCount);
    }

    threadPool_.start();  // 创建了线程并将线程和loop组织成线程池

    for (int i = 0; i < blockSize; ++i) {
      message_.push_back(static_cast<char>(i % 128)); // 构建message, 对一个blockSize, 构造大小的连续长度字符串
    }

    for (int i = 0; i < sessionCount; ++i) {  // 连接数
      char buf[32];
      snprintf(buf, sizeof buf, "C%05d", i);
      Session* session = new Session(threadPool_.getNextLoop(), serverAddr, buf, this);
      session->start(); // 发起connect, 连接成功则回调session::onConnection
      sessions_.emplace_back(session);
    }
  }

    const string& message() const {
      return message_;
    }

    void onConnect() {
      if (numConnected_.incrementAndGet() == sessionCount_) {  // 原子自增
        LOG_WARN << "all connected";
      }
    }

    void onDisconnect(const TcpConnectionPtr& conn) { // 输出测试结果
      if (numConnected_.decrementAndGet() == 0){// 原子递减
        LOG_WARN << "all disconnected";

        int64_t totalBytesRead = 0;
        int64_t totalMessagesRead = 0;
        for (const auto& session : sessions_) { // 所有session传输的数据
          totalBytesRead += session->bytesRead(); // 字节数据
          totalMessagesRead += session->messagesRead(); // 块数据
        }

        LOG_WARN << totalBytesRead << " total bytes read";
        LOG_WARN << totalMessagesRead << " total messages read";
        LOG_WARN << static_cast<double>(totalBytesRead) / static_cast<double>(totalMessagesRead)
                << " average message size";
        LOG_WARN << static_cast<double>(totalBytesRead) / (timeout_ * 1024 * 1024)
                << " MiB/s throughput";

        conn->getLoop()->queueInLoop(std::bind(&Client::quit, this));
      } 
    }

 private:
  void quit() {
    loop_-> queueInLoop(std::bind(&EventLoop::quit, loop_));
  }

  void handleTimeout() {  // 超时终止
    LOG_WARN << "stop";
    for (auto& session : sessions_) {
      session->stop();  // 关闭连接
    }
  }

  EventLoop* loop_;
  EventLoopThreadPool threadPool_;
  int sessionCount_;
  int timeout_;
  std::vector<std::unique_ptr<Session>> sessions_;
  string message_;
  AtomicInt32 numConnected_;
};

void Session::onConnection(const TcpConnectionPtr& conn)  // 连接回调函数, 建立连接和关闭连接时都会调用
{
  if (conn->connected())  // 连接成功回调Client::onConnect()
  {
    conn->setTcpNoDelay(true);
    conn->send(owner_->message());  // 发送message信息
    owner_->onConnect();  // 回调Client::onConnect()
  }
  else  // 关闭连接时也调用
  {
    owner_->onDisconnect(conn);
  }
}


int main(int argc, char* argv[]) {
  if (argc != 7) {
    fprintf(stderr, "Usage: client <host_ip> <port> <threads> <blocksize> ");
    fprintf(stderr, "<sessions> <time> \n");
    abort();
  }
  LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
  Logger::setLogLevel(Logger::WARN);

  const char* ip = argv[1];
  uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
  int threadCount = atoi(argv[3]);
  int blockSize = atoi(argv[4]);
  int sessionCount = atoi(argv[5]);
  int timeout = atoi(argv[6]);

  EventLoop loop;
  InetAddress serverAddr(ip, port);
  Client client(&loop, serverAddr, blockSize, sessionCount, timeout, threadCount);
  loop.loop();  // 该loop用来监听定时器
}

