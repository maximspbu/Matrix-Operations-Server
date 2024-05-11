#include "../include/server.h"

Server::Server(boost::asio::io_service& io_service, short port): io_service_(io_service), acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), socket_(io_service){
    DoAccept();
}

void Server::DoAccept(){
    acceptor_.async_accept(socket_, 
        [this](boost::system::error_code ec){
            if (!ec){
                std::make_shared<Session>(std::move(socket_))->Start();
            }
            DoAccept();
        }
    );
}

Server::~Server(){
    acceptor_.close();
    io_service_.stop();
}
