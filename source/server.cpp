#include "../include/server.h"

Server::Server(io::io_context& io_context, std::uint16_t port): io_context_(io_context),
    acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {}
