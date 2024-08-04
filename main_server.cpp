#include "server/server.h"
#include <iostream>

int main() {
    ServerConfig config;
    config.hostname = "localhost";
    config.port = 82;
    config.connectionOpened = [](const ConnectionInfo& connection){};
    config.connectionClosed = [](const ConnectionInfo& connection) {};
    config.messageHandler = [](const Server* server, const SOCKET sender, const ConnectionInfo& connection, const Message& message){
        std::cout << std::string(message.content);
        Server::sendMessage(sender, message);
    };
    Server server(config);
    server.start();
    return 0;
}
