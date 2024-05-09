#include "../include/client.h"

Client::Client(): socket_(context_){

}

void Client::Run(){
    ReadResponse();
    while (socket_.is_open()){
        std::cout << "Request:\n";
        std::string request;
        std::getline(std::cin, request);
        SendRequest(request);
        ReadResponse();
    }
}

void Client::Connect(const std::string& host, const size_t port){
    boost::asio::ip::tcp::resolver resolver(context_);
    boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));
    connect(socket_, endpoints);
}

void Client::SendRequest(const std::string& request){
    boost::system::error_code ec;
    boost::asio::write(socket_, boost::asio::buffer(request +"\r\n"), ec);
    if (ec){
        std::cerr << "SendRequest error: " << ec.message() << '\n';
        return;
    }
}

void Client::ReadResponse(){
    boost::system::error_code ec;
    buffer_.consume(buffer_.size());
    if (ec){
        if (ec == boost::asio::error::eof){
            std::cerr << "Disconnect\n";
        } else {
            std::cerr << "ReadResponse error:" << ec.message() << '\n';
        }
        return;
    }
    std::istream is(&buffer_);
    std::string response;
    std::getline(is, response, '\r');
    std::cout << "Responce: " << response << '\n';
}
