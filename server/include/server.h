#ifndef SERVER_H
#define SERVER_H

#include "session.h"

class Server{
    public:
        Server(boost::asio::io_service& io_service, short port);
        ~Server();
    private:
        void DoAccept();
        tcp::acceptor acceptor_;
        tcp::socket socket_;
        boost::asio::io_service& io_service_;
};

#endif //SERVER_H
