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
        session(tcp::socket socket);

        void start(){
            do_read();
        }

    private:
        void do_read();
        std::string compute(const std::string& expr);
        void do_write();
        void stop();
        tcp::socket socket_;
        boost::asio::streambuf buf;
        enum { max_length = 1024 };
        std::string data_;
        
};

#endif //SESSION_H
