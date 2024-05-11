#include "../include/session.h"

session::session(tcp::socket socket): socket_(std::move(socket)){

}

void session::do_read(){
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
        data_ = request;
        do_write();
    } catch (const std::exception& e) {
        std::cerr << "Error in Session::do_read(): " << e.what() << std::endl;
        return ;
    }
}

std::string session::compute(const std::string& expr){
    Tree tree(expr); //fix cos(0)
    return tree.MultithreadCompute();
}

void session::do_write(){
    std::string output;
    boost::system::error_code ec;
    try {
        output = compute(data_); //data_.substr(0, data_.size() - 1)
    } catch (std::exception& e) {
        std::cerr << "Error: tree error\n";
        return ;
    }
    std::cout << output << '\n';
    boost::asio::write(socket_, boost::asio::buffer(output + '\n'), ec);
    if (ec){
        std::cerr << "Error do_write: " << ec.message() << '\n';
        exit(0);
    }
    do_read();
}

void session::stop(){
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket_.close();
}
