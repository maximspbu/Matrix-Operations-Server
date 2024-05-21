#include "../include/session.h"

Session::Session(tcp::socket socket): socket_(std::move(socket)){

}

void Session::Start(){
    DoRead();
}

void Session::DoRead(){
    try {
        auto self(shared_from_this());
        
        boost::system::error_code ec;
        
        boost::asio::async_read_until(socket_, buf_, "\n", 
        [this, self](const boost::system::error_code& ec, size_t){
            if (ec) {
                Stop();
                return ;
            }

            std::istream is(&buf_);
            std::string request;
            std::getline(is, request);
            std::cout << request << '\n';
            if (request == "quit"){
                Stop();
                return;
            }
            if (request != ""){
                data_ = request;
                std::getline(is, request);
                size_t countMatrix;
                Convert<size_t>("matricies count", request, countMatrix, atoi);
                std::cout << "Matrcies count: " << countMatrix << '\n';
                for (size_t i = 0; i < countMatrix; ++i){
                    std::getline(is, request, ' ');
                    std::string matrixName = request;
                    std::cout << "Matrix name: " << matrixName << '\n';
                    std::getline(is, request, ' ');
                    size_t rowSize;
                    Convert<size_t>("row size", request, rowSize, atoi);
                    std::cout << "Row size: " << rowSize << '\n';
                    std::getline(is, request, '[');
                    size_t columnSize;
                    Convert<size_t>("column size", request, columnSize, atoi);
                    std::cout << "Column size: " << columnSize << '\n';
                    std::vector<double> elems;
                    for (size_t j = 0; j < rowSize*columnSize - 1; ++j){
                        std::getline(is, request, ' ');
                        std::cout << "Elem: " << request << '\n';
                        try {
                            elems.push_back(std::stod(request.c_str()));
                        } catch (std::exception& e){
                            std::cerr << "Incorrect type of element!\n";
                            Stop();
                            return ;
                        }
                    }
                    std::getline(is, request, ']');
                    std::cout << "Elem: " << request << '\n';
                    elems.push_back(std::stod(request.c_str()));
                    matricies_.emplace_back(matrixName, rowSize, columnSize, elems);
                    std::getline(is, request, '\n');
                }
                DoWrite();
            }
            DoRead();
        });
    } catch (const std::exception& e) {
        std::cerr << "Error in Session::DoRead(): " << e.what() << std::endl;
        return ;
    }
}

std::string Session::Compute(const std::string& expr){
    std::map<std::string, boost::numeric::ublas::matrix<double>> matricies;
    for (auto& matrix: matricies_){
        boost::numeric::ublas::matrix<double> m(matrix.rowSize, matrix.columnSize);
        for (size_t i = 0; i < matrix.rowSize; ++i){
            for (size_t j = 0; j < matrix.columnSize; ++j){
                m(i, j) = matrix.values[matrix.columnSize*i + j];
            }
        }
        matricies[matrix.name] = m;
    }
    Tree tree(expr, matricies);
    return (tree.GetErrorString().size() == 0)? tree.MultithreadCompute(): tree.GetErrorString();
}

void Session::DoWrite(){
    std::string output;
    try {
        output = '\n' + Compute(data_);
        output += '\t';
    } catch (std::exception& e) {
        std::cerr << "Error: tree error\n";
        return ;
    }
    std::cout << output << '\n';
    boost::asio::async_write(socket_, boost::asio::buffer(output), 
    [this](const boost::system::error_code& ec, size_t){
        if (ec) {
            std::cerr << "Error DoWrite: " << ec.message() << '\n';
            exit(0);
        }
    });
    matricies_.clear();
}

void Session::Stop(){
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (!ec || ec == boost::asio::error::eof) {
        boost::asio::write(socket_, boost::asio::buffer("End of session!\n"), ec);
    }
    socket_.close();
}
