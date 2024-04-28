#ifndef SERVER_H
#define SERVER_H

#include "session.h"

class Server{
    public:
        Server(io::io_context& io_context, std::uint16_t port);
        void AsyncAccept();
        void GetResult();
    private:
        io::io_context& io_context_;
        tcp::acceptor acceptor_;
        std::optional<tcp::socket> socket_;
        std::unordered_set<std::shared_ptr<Session>> clients_;
};

#endif //SERVER_H
