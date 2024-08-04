#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <unordered_map>
#include <netdb.h>
#include "../message_length.h"
#include "../connection_info.h"
#include "../socket.h"
#include "server_config.h"
#include "../message.h"

#define BACKLOG 10

class Server {
public:
    Server(const ServerConfig config): config(config) {}

    Server(const Server& other) = delete;
    Server& operator=(const Server* other) = delete;

    ~Server();
public:
    int start() noexcept;
    void stop() noexcept;
    static int sendMessage(SOCKET recipient, Message message);
private:
    SOCKET createServerSocket(addrinfo* bind_address) noexcept;
    int configureServerSocket(SOCKET server_socket) noexcept;
    int broadcastMessage(Message message) noexcept;
    Message* receiveMessage(SOCKET sender_socketfd) noexcept;
    int acceptConnection() noexcept;
    void disconnectClient(SOCKET sockfd) noexcept;
    int handleConnections() noexcept;
private:
    const ServerConfig config;

    SOCKET server_socket;

    SOCKET max_socket;
    fd_set sock_polling_set;

    std::unordered_map<SOCKET, ConnectionInfo> connected_clients;
};
#endif