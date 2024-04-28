#ifndef SERVER_H
#define SERVER_H

#include "session.h"

class Server{
    public:
        Server();
        void AsyncAccept();
        void GetResult();
    private:
        tcp::acceptor acceptor;
        std::optional<tcp::socket> socket;
        std::unordered_set<std::shared_ptr<Session>> clients;
};

#endif //SERVER_H
