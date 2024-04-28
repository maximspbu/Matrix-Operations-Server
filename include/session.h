#ifndef SESSION_H
#define SESSION_H

#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <queue>
#include <optional>
#include <unordered_set>
#include <boost/asio.hpp>
#include <boost/numeric/ublas/matrix.hpp>

namespace io = boost::asio;
using tcp = io::ip::tcp;
using error_code = boost::system::error_code;
using namespace std::placeholders;

using message_handler = std::function<void(std::string)>;
using error_handler = std::function<void()>;

class Session: public std::enable_shared_from_this<Session>{
    public:
        Session(tcp::socket&& socket);
        void Start();
        void AsyncRead();
        void OnRead();
        void BuildOperationsPriopity();
        void Calculate();
    private:
        tcp::socket socket_;
        io::streambuf streambuf_;
        std::queue<std::string> outgoing_;
        message_handler on_message_;
        error_handler on_error_;
};

#endif //SESSION_H
