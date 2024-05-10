#include "../include/session.h"

session::session(tcp::socket socket): socket_(std::move(socket)){

}

void session::do_read(){
    if (!socket_.is_open()){
        std::cerr << "Socket is closed\n";
    }
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length), 
        [this, self](boost::system::error_code ec, std::size_t length){
            if (data_ == "quit"){
                stop();
            }
            if (!ec){
                do_write(length);
            } else {
                std::cerr << "Error: do_read " << ec.message() << '\n';
                stop();
            }
        }
    );
    // data_ = "2+2";
    // std::cout << "flag\n";
    
}

std::string session::compute(const std::string& expr){
    Tree tree(expr);
    tree.fill();
    return tree.MultithreadCompute();
}

void session::do_write(std::size_t length){
    std::string output = compute(data_);
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(output, output.size()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/){
            if (!ec){
                do_read();
            } else {
                std::cerr << "Error: do_write " << ec.message() << '\n';
                stop();
            }
        }
    );
}

void session::stop(){
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket_.close();
    exit(0); 
}
