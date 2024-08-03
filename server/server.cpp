#include "server.h"
#include <sys/types.h>
#include "../logging.h"
#include <cstring>
#include "../connection.h"

int Server::start() noexcept {
    addrinfo* server_addr_struct = resolveConnectionAddress(hostname, port);
    if (!server_addr_struct) {
        return -1;
    }

    server_socket = createServerSocket(server_addr_struct);
    if (!IS_VALID_SOCKET(server_socket)) {
        return -2;
    }

    freeaddrinfo(server_addr_struct);

    FD_ZERO(&sock_polling_set);
    FD_SET(server_socket, &sock_polling_set);
    max_socket = server_socket;

    return handleConnections();
}

void Server::stop() noexcept {
    INFO("Shutting down the server");
    CLOSE_SOCKET(server_socket);
    INFO("Server is shutted down");
}

int Server::createServerSocket(addrinfo *bind_address) noexcept {
    server_socket = createSocket(bind_address);

    DEBUG("Binding socket to the resolved address");
    if (bind(server_socket, bind_address->ai_addr, bind_address->ai_addrlen) == -1) {
        ERROR(-3, "Failed to bind server socket to address " + hostname + ":" << port);
    }

    if(configureServerSocket(server_socket) < 0) {
        return -4;
    }

    return server_socket;
}

int Server::configureServerSocket(SOCKET server_socket) noexcept {
    int yes = 1;

    DEBUG("Setting SO_REUSEADDR socket option to the server socket");
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        ERROR(-1, "Failed to set socket options: setsockopt(): " + std::system_category().message(GET_SOCKET_ERRNO()));
    }

    DEBUG("Activating server listening mode");
    if (listen(server_socket, BACKLOG) == -1) {
        ERROR(-2, "Failed to activate socket listener: listen(): " + std::system_category().message(GET_SOCKET_ERRNO()));
    }

    INFO("Server is listening for incoming connections at " + hostname + ":" << port);
    return 0;
}

void Server::disconnectClient(SOCKET sockfd) noexcept{
    const ConnectionInfo& conn_info = connected_clients.at(sockfd);
    INFO("Client " << conn_info.toString() << " has been disconnected.");

    CLOSE_SOCKET(sockfd);
    connected_clients.erase(sockfd);
    FD_CLR(sockfd, &sock_polling_set);
}

int Server::handleConnections() noexcept{
    while (true){
        fd_set polling_set_copy = sock_polling_set;

        if (select(max_socket + 1, &polling_set_copy, NULL, NULL, NULL) < 0){
            ERROR(-1, "Failed to fetch data on the server socket: select(): " << std::system_category().message(GET_SOCKET_ERRNO()));
        }

        for (SOCKET sock = 1; sock <= max_socket + 1; ++sock){
            if (FD_ISSET(sock, &polling_set_copy)){
                if (sock == server_socket){
                    if (acceptConnection() == -1){
                        return -1;
                    }
                } else {
                    char msg_buffer[MESSAGE_MAX_SIZE];
                    memset(msg_buffer, 0x00, MESSAGE_MAX_SIZE);

                    int recv_bytes = receiveMessage(sock, msg_buffer);

                    if (recv_bytes <= 0){
                        disconnectClient(sock);
                        continue;
                    }

                    broadcastMessage(msg_buffer, recv_bytes);
                }
            }
        }
    }
    return 0;
}

int Server::acceptConnection() noexcept{
    sockaddr_storage conn_addr;
    socklen_t conn_len = sizeof(conn_addr);

    SOCKET new_conn = accept(server_socket, reinterpret_cast<sockaddr*>(&conn_addr), &conn_len);
    ConnectionInfo new_conn_info = getConnectionInfo(&conn_addr);
    if (!IS_VALID_SOCKET(new_conn)){
        std::cerr << "[Error] Failed to accept new connection from " << new_conn_info.toString() << " : accept(): " << std::system_category().message(GET_SOCKET_ERRNO()) << std::endl;
        return -1;
    }

    std::cout << "[Info] New connection from " << new_conn_info.toString() << '\n';

    if (new_conn > max_socket){
        max_socket = new_conn;
    }
    FD_SET(new_conn, &sock_polling_set);
    connected_clients[new_conn] = new_conn_info;
    return 0;
}

int Server::broadcastMessage(const void* message, MESSAGE_LENGTH_TYPE length) noexcept{
    for (const auto& [sock, client_info] : connected_clients){
        if (sendMessage(sock, message, length) == -1){
            disconnectClient(sock);
            return -1;
        }
    }
    return 0;
}

int Server::receiveMessage(SOCKET sender_socketfd, char* writable_buffer) noexcept{
    return ::receiveMessage(sender_socketfd, writable_buffer, &connected_clients);
}

Server::~Server() = default;