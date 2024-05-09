#ifndef SESSION_H
#define SESSION_H

#include "tree.h"

#include <ctime>
#include <memory>
#include <optional>
#include <boost/asio.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <string>

using boost::asio::ip::tcp;

class session: public std::enable_shared_from_this<session>{
    public:
        session(tcp::socket socket): socket_(std::move(socket)){

        }

        void start(){
            do_read();
        }

    private:
        void do_read();

        void convert_expression(std::vector<std::string> request);

        void calculate_expression();

        void display_result(std::size_t length);

        std::string compute(const std::string& expr);

        void do_write(std::size_t length);

        tcp::socket socket_;
        enum { max_length = 1024 };
        char data_[max_length];
        
};

#endif //SESSION_H
