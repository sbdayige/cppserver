#pragma once
#include <functional>
#include "Buffer.h"

class EventLoop;
class Socket;
class Channel;

class Connection
{
private:
    EventLoop *loop;
    Socket *sock;
    Channel *channel;
    Buffer *readBuffer;
    Buffer *sendBuffer;
    std::function<void(Socket *)> deleteConnectionCallback;
    std::function<void(Connection *)> onConnectionCallback;

public:
    enum class State
    {
        Connected,
        Closed
    } state;
    Connection(EventLoop *_loop, Socket *_sock);
    ~Connection();

    // Event callback used by Channel
    void handleEvent();

    // Read / Write API for user logic
    void Read();
    void Write();

    // Helpers
    const char *ReadBuffer();
    ssize_t ReadBufferSize();
    void SetSendBuffer(const char *data, ssize_t len);
    void GetlineSendBuffer();
    Socket *GetSocket();
    State GetState();
    void Close();

    void setDeleteConnectionCallback(std::function<void(Socket *)> _cb);
    void setOnConnectionCallback(std::function<void(Connection *)> _cb);
};
