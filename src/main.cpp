#include "server/Server.h"
#include <iostream>

int main() {
    try {
        Server server(8080);
        server.Start();
    } catch (const std::exception& e) {
        std::cerr << "Server crashed: " << e.what() << '\n';
        return 1;
    }
    return 0;
}