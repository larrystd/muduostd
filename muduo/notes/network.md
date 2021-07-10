### 网络连接的基本过程
一个简单的Tcp服务器时，建立一个新的连接通常需要四步：

步骤1. socket() // 调用socket函数建立监听socket    
步骤2. bind()   // 绑定地址和端口 
步骤3. listen() // 开始监听端口 
步骤4. accept() // 返回新建立连接的fd

在muduo中，首先在TcpServer对象构建时，TcpServer的属性acceptor同时也被建立。 在Acceptor的构造函数中分别调用了socket函数和bind函数完成了步骤1和步骤2。
```cpp
Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
  : loop_(loop),
    acceptSocket_(sockets::createNonblockingOrDie(listenAddr.family())),
    acceptChannel_(loop, acceptSocket_.fd()),
    listening_(false),
    idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
  assert(idleFd_ >= 0);
  acceptSocket_.setReuseAddr(true);
  acceptSocket_.setReusePort(reuseport);
  acceptSocket_.bindAddress(listenAddr);
  acceptChannel_.setReadCallback(
      std::bind(&Acceptor::handleRead, this));
}
```
即，当TcpServer server(&loop, listenAddr);执行结束时，监听socket已经建立好，并已绑定到对应地址和端口了。

而当执行server.start()时，主要做了两个工作：

1. 在监听socket上启动listen函数，也就是步骤3；

2. 将监听socket的可读事件注册到EventLoop中。

此时，程序已完成对地址的监听， 当调用loop.loop()时，程序开始监听该socket的可读事件。
```cpp
void TcpServer::start()
{
  if (started_.getAndSet(1) == 0)
  {
    threadPool_->start(threadInitCallback_);

    assert(!acceptor_->listening());
    loop_->runInLoop(
        std::bind(&Acceptor::listen, get_pointer(acceptor_)));
  }
}
```
当新连接请求建立时，可读事件触发，此时该事件对应的callback在EventLoop::loop()中被调用。 该事件的callback实际上就是Acceptor::handleRead()方法。

在Acceptor::handleRead()方法中，做了三件事：

1. 调用了accept函数，完成了步骤4，实现了连接的建立。得到一个已连接socket的fd,

2. 创建TcpConnection对象,

3. 将已连接socket的可读事件注册到EventLoop中。


### 消息的获取
如客户端发送消息，导致已连接socket的可读事件触发，该事件对应的callback同样也会在EventLoop::loop()中被调用。

该事件的callback实际上就是TcpConnection::handleRead方法。 在TcpConnection::handleRead方法中，主要做了两件事：

1. 从socket中读取数据，并将其放入inputbuffer中

2. 调用messageCallback，执行业务逻辑。

```cpp
ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
if (n > 0)
{
    messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
}

```
TcpServer::messageCallback就是业务逻辑的主要实现函数。
在messageCallback中，用户会有可能会把任务抛给自定义的Worker线程池处理。 但是这个在Worker线程池中任务，切忌直接对Buffer的操作。因为Buffer并不是线程安全的。因此，所有对IO和buffer的读写，都应该在IO线程中完成。



