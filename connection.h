#ifndef CONNECTION_H
#define CONNECTION_H

#include <netdb.h>
#include <cstring>
#include <unordered_map>
#include "connection_info.h"
#include "socket.h"
#include "logging.h"

addrinfo* resolveConnectionAddress(std::string hostname, in_port_t port) noexcept{
    DEBUG("Resolving connection address");

    addrinfo hints, *bind_address;
    std::memset(&hints, 0x00, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    DEBUG("Resolving server hostname");
    if (getaddrinfo(hostname.data(), std::to_string(port).data(), &hints, &bind_address) != 0) {
        ERROR(nullptr, "Failed to resolve server's local address: getaddrinfo(): " + std::system_category().message(GET_SOCKET_ERRNO()));
    }

    return bind_address;
}

SOCKET createSocket(addrinfo* conn_addr) noexcept {
    if (!conn_addr) {
        ERROR(-1, "CreateServerSocket(): bind_address is NULL");
    }

    DEBUG("Creating server socket object");
    SOCKET server_socket = socket(conn_addr->ai_family, conn_addr->ai_socktype, conn_addr->ai_protocol);
    if (!IS_VALID_SOCKET(server_socket)) {
        ERROR(-2, "Failed to create server socket: socket() " + std::system_category().message(GET_SOCKET_ERRNO()));
    }

    return server_socket;
}

int sendMessage(SOCKET socket, const void* message, MESSAGE_LENGTH_TYPE length) noexcept{
    char* finalMessage = (char*) prependMessageLength(message, length);

    int total_bytes = length + MESSAGE_LENGTH_PREFIX_SIZE;
    int sent_bytes = 0;
    int sent_n;

    while (total_bytes > sent_bytes){
        sent_n = send(socket, finalMessage + sent_bytes, total_bytes - sent_bytes, 0);
        if (sent_n == -1){
            ERROR(sent_n, "Failed to send data to the remote host: send(): " << std::system_category().message(GET_SOCKET_ERRNO()));
        }
        sent_bytes += sent_n;
        DEBUG("Sending " << std::to_string(sent_n) << " bytes");
    }


    return sent_bytes;
}

int receiveMessage(SOCKET socket, char* writable_buffer, std::unordered_map<SOCKET, ConnectionInfo>* clients) noexcept{
    if (!writable_buffer){
        ERROR(-1, "receiveMessage(): writable_buffer is NULL.");
    }

    std::string conn_info_description;
    if (clients != nullptr) {
        conn_info_description = clients->at(socket).toString();
    }

    MESSAGE_LENGTH_TYPE message_length;

    int recv_bytes = recv(socket, &message_length, MESSAGE_LENGTH_PREFIX_SIZE, 0);
    if (recv_bytes <= 0){
        if (recv_bytes < 0){
            ERROR(recv_bytes, "Failed to receive message length from " << conn_info_description << " : recv(): " << std::system_category().message(GET_SOCKET_ERRNO()));
        }
        return 0;
    }
    DEBUG(conn_info_description << " (length): '" << message_length);

    recv_bytes = recv(socket, writable_buffer, message_length, 0);
    if (recv_bytes <= 0){
        if (recv_bytes < 0){
            std::cerr << "[Error] Failed to receive message from " << conn_info_description << " : recv(): " << std::system_category().message(GET_SOCKET_ERRNO()) << std::endl;
        }
    }

    DEBUG(conn_info_description << " (" << message_length << " bytes): '" << writable_buffer);
    return recv_bytes;
}

#endif