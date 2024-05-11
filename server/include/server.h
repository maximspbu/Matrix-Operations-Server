#ifndef SERVER_H
#define SERVER_H

#include "session.h"

class Server{
    public:
        Server(boost::asio::io_service& io_service, short port);

    private:
        void DoAccept();
        //~server();
        tcp::acceptor acceptor_;
        tcp::socket socket_;
};

#endif //SERVER_H
