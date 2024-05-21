#ifndef SERVER_H
#define SERVER_H

#include "session.h"

class Server{
public:
    Server(boost::asio::io_service& io_service, short port);
    ~Server();
private:
    void DoAccept();
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    boost::asio::io_service& io_service_;
    template <typename T, typename U>
    struct wrapper{
        std::map<std::string, std::pair<std::function<T(U, T)>, bool>> map_functions;
    };

    wrapper<double, double> wrap_;
    wrapper<boost::numeric::ublas::matrix<double>, double> matrix_to_double_;
    wrapper<boost::numeric::ublas::matrix<double>, boost::numeric::ublas::matrix<double>> matrix_to_matrix_;
};

#endif //SERVER_H
