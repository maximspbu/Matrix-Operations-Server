#include "../include/server.h"

int main(){   
    try 
    {
        boost::asio::io_service io_service;
        Server s(io_service, 8080);
        io_service.run();

    } catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}
