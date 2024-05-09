#include "../include/session.h"

void session::do_read(){
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length), 
        [this, self](boost::system::error_code ec, std::size_t length){
            if (!ec){
                do_write(length);
            }
        }
    );
}

void session::convert_expression(std::vector<std::string> request){
    
    //params: infix notation string
    //call calculate_expression, distribute threads to calculate
    //returns: result (matrix, number, e.t.)
}

void session::calculate_expression(){
    //params: request, used variables, array of function pointers
    //returns: result (matrix, number, e.t.)

}

void session::display_result(std::size_t length){
    
}



std::string session::compute(const std::string& expr){
    Tree tree(expr);
    tree.fill();
    return tree.MultithreadCompute();
}

void session::do_write(std::size_t length){
    std::cout << "flag\n";
    std::string input = data_;
    std::cout << "flag\n";
    std::string output = compute(input);
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(output, length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/){
            if (!ec){
                do_read();
            }
        }
    );
}
