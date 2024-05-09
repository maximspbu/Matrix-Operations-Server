#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio.hpp>
#include <string>
#include <iostream>

class Client{
public:
    Client();
    void Run();
    void Connect(const std::string& host, size_t port);
private:
    void SendRequest(const std::string& request);
    void ReadResponse();
    boost::asio::io_context context_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf buffer_;
};

#endif // CLIENT_H
