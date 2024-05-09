#include "../include/client.h"

int main(){
    Client client;
    client.Connect("localhost", 8080);
    client.Run();
}
