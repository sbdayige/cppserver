#include "include/EventLoop.h"
#include "include/Server.h"
#include "include/Connection.h"
#include "include/Socket.h"
#include <iostream>

int main()
{
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);

    server->OnConnect([](Connection *conn)
                      {
        conn->Read();
        if (conn->GetState() == Connection::State::Closed)
        {
            conn->Close();
            return;
        }
        std::cout << "Message from client " << conn->GetSocket()->getFd() << ": " << conn->ReadBuffer() << std::endl;
        conn->SetSendBuffer(conn->ReadBuffer(), conn->ReadBufferSize());
        conn->Write(); });

    loop->loop();

    delete server;
    delete loop;
    return 0;
}