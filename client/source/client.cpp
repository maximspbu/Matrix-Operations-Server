#include "../include/client.h"

Client::Client(): socket_(context_){

}

void Client::Run(){
    while (socket_.is_open()){
        std::string response;
        std::getline(std::cin, response);
        if (response != "quit"){
            std::cout << "How many matricies?\n";
            std::string countMatrix;
            std::getline(std::cin, countMatrix);
            response += '\n' + countMatrix + '\n';
            if (atoi(countMatrix.c_str()) > 0){
                std::cout << "Enter matrix like: <name> <row size> <column size> [<num> <num> <num>]\n";
                for (size_t i = 0; i < atoi(countMatrix.c_str()); ++i){
                    std::string matrixString;
                    std::getline(std::cin, matrixString);
                    response += matrixString + '\n';
                }
            }
        }
        SendRequest(response);
        ReadResponse();
        if (!socket_.is_open()){
            std::cerr << "Socket is closed!\n";
            return ;
        }
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
    boost::asio::write(socket_, boost::asio::buffer(request + "\n"), ec);
    if (ec){
        std::cerr << "SendRequest error: " << ec.message() << '\n';
        exit(0);
    }
}

void Client::ReadResponse(){
    boost::system::error_code ec;

    read_until(socket_, buffer_, "\t", ec);

    if (ec) {
        throw boost::system::system_error(ec);
    }

    std::istream is(&buffer_);
    std::string response;
    std::getline(is, response, '\t');
    std::cout << "Response: " << response << '\n';
}
