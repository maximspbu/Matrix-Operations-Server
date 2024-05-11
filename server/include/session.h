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

class Session: public std::enable_shared_from_this<Session>{
    public:
        Session(tcp::socket socket);

        void Start();

    private:
        void DoRead();
        std::string Compute(const std::string& expr);
        void DoWrite();
        void Stop();

        
        tcp::socket socket_;
        boost::asio::streambuf buf_;
        enum { max_length = 1024 };
        std::string data_;
        
};

#endif //SESSION_H
