#include "include/Connection.h"
#include "include/Socket.h"
#include "include/Channel.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include "tools/tools.h"

#define READ_BUFFER 1024

static bool isNonBlockingFd(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    return flags & O_NONBLOCK;
}

Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock), channel(nullptr), readBuffer(nullptr), sendBuffer(nullptr)
{
    readBuffer = new Buffer();
    sendBuffer = new Buffer();
    state = State::Connected;
    if (loop)
    {
        channel = new Channel(loop, sock->getFd());
        std::function<void()> cb = std::bind(&Connection::handleEvent, this);
        channel->setCallback(cb);
        channel->enableReading();
    }
}

Connection::~Connection()
{
    if (channel)
        delete channel;
    delete sock;
    delete readBuffer;
    delete sendBuffer;
}

void Connection::handleEvent()
{
    if (onConnectionCallback)
    {
        onConnectionCallback(this);
    }
}

void Connection::Read()
{
    if (state != State::Connected)
        return;
    readBuffer->clear();
    int fd = sock->getFd();
    if (isNonBlockingFd(fd))
        ; // fall through to non-blocking read below

    // perform non-blocking style read loop
    char buf[READ_BUFFER];
    while (true)
    {
        bzero(&buf, sizeof(buf));
        ssize_t n = ::read(fd, buf, sizeof(buf));
        if (n > 0)
        {
            readBuffer->append(buf, n);
            // continue reading until EAGAIN
            continue;
        }
        else if (n == -1 && errno == EINTR)
        {
            continue;
        }
        else if (n == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            break; // done reading
        }
        else if (n == 0)
        {
            state = State::Closed;
            break;
        }
        else
        {
            // other error
            perror("read");
            state = State::Closed;
            break;
        }
    }
}

void Connection::Write()
{
    if (state != State::Connected)
        return;
    int fd = sock->getFd();
    const char *data = sendBuffer->c_str();
    ssize_t total = sendBuffer->size();
    ssize_t written = 0;
    while (written < total)
    {
        ssize_t n = ::write(fd, data + written, total - written);
        if (n > 0)
        {
            written += n;
            continue;
        }
        else if (n == -1 && errno == EINTR)
        {
            continue;
        }
        else if (n == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            // can't write more now
            break;
        }
        else
        {
            perror("write");
            state = State::Closed;
            break;
        }
    }
    // clear send buffer regardless (simple behavior)
    sendBuffer->clear();
}

const char *Connection::ReadBuffer()
{
    return readBuffer->c_str();
}

ssize_t Connection::ReadBufferSize()
{
    return readBuffer->size();
}

void Connection::SetSendBuffer(const char *data, ssize_t len)
{
    sendBuffer->clear();
    sendBuffer->append(data, len);
}

void Connection::GetlineSendBuffer()
{
    sendBuffer->clear();
    sendBuffer->getline();
}

Socket *Connection::GetSocket()
{
    return sock;
}

Connection::State Connection::GetState()
{
    return state;
}

void Connection::Close()
{
    deleteConnectionCallback(sock);
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket *)> _cb)
{
    deleteConnectionCallback = _cb;
}

void Connection::setOnConnectionCallback(std::function<void(Connection *)> _cb)
{
    onConnectionCallback = _cb;
}
