#ifndef SESSION_H
#define SESSION_H

#include "tree.h"

#include <boost/asio.hpp>

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
    template<typename T>
    void Convert(const std::string& name, const std::string& str, T& output, std::function<T(const char*)> f){
        try {
            output = f(str.c_str());
        } catch (std::exception& e) {
            std::cerr << "Incorrect type of " << name << '\n';
            Stop();
            return ;
        }
    }

    tcp::socket socket_;
    boost::asio::streambuf buf_;
    enum { max_length = 1024 };
    std::string data_;
    struct Matrix{
        std::string name;
        size_t rowSize;
        size_t columnSize;
        std::vector<double> values;
    };
    std::vector<Matrix> matricies_;
};

#endif //SESSION_H
