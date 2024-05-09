#include "../include/server.h"

int main(){   
    int a = 2;
    std::cerr << a << '\n';
    try{
        if (a != 2){
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        server s(io_service, 8001);

        io_service.run();
    } catch (std::exception& e){
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
