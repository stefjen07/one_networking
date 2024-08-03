#ifndef CONNECTION_INFO_H
#define CONNECTION_INFO_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

struct ConnectionInfo {
    bool success;
    std::string address;
    in_port_t port;

    std::string toString() const noexcept {
        return address + ":" + std::to_string(port);
    }
};

inline static ConnectionInfo getConnectionInfo(sockaddr_storage* addr) {
    sockaddr_in* conn_addr = reinterpret_cast<sockaddr_in*>(addr);

    ConnectionInfo ret_conn;
    ret_conn.success = false;

    char ip_addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &conn_addr->sin_addr, ip_addr, INET_ADDRSTRLEN);

    ret_conn.address = std::string(ip_addr);
    ret_conn.port = conn_addr->sin_port;
    ret_conn.success = true;

    return ret_conn;
}

#endif