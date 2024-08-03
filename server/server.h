#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <unordered_map>
#include <netdb.h>
#include "../message_length.h"
#include "../connection_info.h"
#include "../socket.h"

#define BACKLOG 10

class Server {
public:
    Server(const char* hostname, in_port_t port): hostname(hostname), port(port) {}

    Server(const Server& other) = delete;
    Server& operator=(const Server* other) = delete;

    ~Server();
public:
    int start() noexcept;
    void stop() noexcept;
private:
    SOCKET createServerSocket(addrinfo* bind_address) noexcept;
    int configureServerSocket(SOCKET server_socket) noexcept;
    int broadcastMessage(const void* message, MESSAGE_LENGTH_TYPE length) noexcept;
    int receiveMessage(SOCKET sender_socketfd, char* writable_buffer) noexcept;
    int acceptConnection() noexcept;
    void disconnectClient(SOCKET sockfd) noexcept;
    int handleConnections() noexcept;
private:
    const std::string hostname;
    in_port_t port;

    SOCKET server_socket;

    SOCKET max_socket;
    fd_set sock_polling_set;

    std::unordered_map<SOCKET, ConnectionInfo> connected_clients;
};
#endif