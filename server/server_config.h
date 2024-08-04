#include <string>
#include <functional>
#include "../connection_info.h"
#include "../message.h"

class Server;

struct ServerConfig {
    std::string hostname;
    in_port_t port;

    std::function<void(ConnectionInfo)> connectionOpened;
    std::function<void(ConnectionInfo)> connectionClosed;
    std::function<void(Server*, SOCKET, ConnectionInfo, Message)> messageHandler;
};