#include "../include/client.h"

Client::Client(): socket_(context_){

}

void Client::Run(){
    while (socket_.is_open()){
        std::string response;
        std::getline(std::cin, response);
        SendRequest(response);
        ReadResponse();
    }
}

void Client::Connect(const std::string& host, const size_t port){
    boost::asio::ip::tcp::resolver resolver(context_);
    boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));
    try {
        connect(socket_, endpoints);
    } catch (boost::system::system_error const& e) {
        std::cout << "Warning: could not connect : " << e.what() << '\n';
        exit(0);
    }
}

void Client::SendRequest(const std::string& request){
    boost::system::error_code ec;
    boost::asio::write(socket_, boost::asio::buffer(request +"\n"), ec);
    if (ec){
        std::cerr << "SendRequest error: " << ec.message() << '\n';
        exit(0);
    }
}

void Client::ReadResponse(){
    boost::system::error_code ec;
    boost::asio::streambuf buf;

    read_until(socket_, buf, "\n", ec);

    if (ec) {
        throw boost::system::system_error(ec);
    }

    std::istream is(&buf);
    std::string response;
    std::getline(is, response);
    std::cout << "Response: " << response << '\n';
}
