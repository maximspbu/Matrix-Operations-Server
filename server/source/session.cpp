#include "../include/session.h"

Session::Session(tcp::socket socket): socket_(std::move(socket)){

}

void Session::Start(){
    DoRead();
}

void Session::DoRead(){
    try {
        boost::system::error_code ec;
        boost::asio::streambuf buf;

        read_until(socket_, buf, "\n", ec);

        if (ec) {
            throw boost::system::system_error(ec);
        }

        std::istream is(&buf);
        std::string request;
        std::getline(is, request);
        std::cout << request << '\n';
        if (request == "quit"){
            Stop();
            return;
        }
        data_ = request;
        DoWrite();

    } catch (const std::exception& e) {
        std::cerr << "Error in Session::DoRead(): " << e.what() << std::endl;
        return ;
    }
}

std::string Session::Compute(const std::string& expr){
    Tree tree(expr);
    return tree.MultithreadCompute();
}

void Session::DoWrite(){
    std::string output;
    boost::system::error_code ec;
    try {
        output = Compute(data_);
    } catch (std::exception& e) {
        std::cerr << "Error: tree error\n";
        return ;
    }
    std::cout << output << '\n';
    boost::asio::write(socket_, boost::asio::buffer(output + '\n'), ec);
    if (ec){
        std::cerr << "Error DoWrite: " << ec.message() << '\n';
        exit(0);
    }
    DoRead();
}

void Session::Stop(){
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (!ec || ec == boost::asio::error::eof) {
        boost::asio::write(socket_, boost::asio::buffer("End of session!\n"), ec);
    }
    socket_.close();
    
}
