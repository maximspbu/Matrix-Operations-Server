#include "../include/session.h"

Session::Session(tcp::socket socket): socket_(std::move(socket)){

}

void Session::Start(){
    DoRead();
}

void Session::DoRead(){
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
        if (request == "quit"){
            Stop();
            return;
        }
        data_ = request;
        std::getline(is, request);
        size_t countMatrix = atoi(request.c_str());
        std::cout << countMatrix << '\n';
        for (size_t i = 0; i < countMatrix; ++i){
            std::getline(is, request, ' ');
            std::string matrixName = request;
            std::cout << "Matrix name: " << matrixName << '\n';
            std::getline(is, request, ' ');
            size_t rowSize = atoi(request.c_str());
            std::cout << "Row size: " << rowSize << '\n';
            std::getline(is, request, '[');
            size_t columnSize = atoi(request.c_str());
            std::cout << "Column size: " << columnSize << '\n';
            std::vector<double> elems;
            for (size_t j = 0; j < rowSize*columnSize - 1; ++j){
                std::getline(is, request, ' ');
                std::cout << "Elem: " << request << '\n';
                elems.push_back(atoi(request.c_str()));
            }
            std::getline(is, request, ']');
            std::cout << "Elem: " << request << '\n';
            elems.push_back(atoi(request.c_str()));
            matricies_.emplace_back(matrixName, rowSize, columnSize, elems);
            std::getline(is, request, '\n');
        }
        DoWrite();

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
                std::cout << m(i, j) << '\n';
            }
        }
        for (size_t i = 0; i < matrix.values.size(); ++i){
            std::cout << matrix.values[i] << ' ';
        }
        std::cout << '\n';
        matricies[matrix.name] = m; //matrix.values
    }
    Tree tree(expr, matricies);
    return tree.MultithreadCompute();
}

void Session::DoWrite(){
    std::string output;
    boost::system::error_code ec;
    try {
        output = Compute(data_);
    } catch (std::exception& e) {
        std::cerr << "Error: tree error\n";
        return ;
    }
    std::cout << output << '\n';
    boost::asio::write(socket_, boost::asio::buffer(output + '\n'), ec);
    if (ec){
        std::cerr << "Error DoWrite: " << ec.message() << '\n';
        exit(0);
    }
    matricies_.clear();
    DoRead();
}

void Session::Stop(){
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (!ec || ec == boost::asio::error::eof) {
        boost::asio::write(socket_, boost::asio::buffer("End of session!\n"), ec);
    }
    socket_.close();
    
}
