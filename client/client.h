#ifndef CLIENT_H
#define CLIENT_H

#include "../socket.h"
#include "../message.h"
#include <string>
#include <netdb.h>

class Client{
public:
    Client(const char* hostname, in_port_t port) : hostname(hostname), port(port) {}

    Client(const Client& other) = delete;
    Client& operator=(const Client& other) = delete;

    ~Client(){
        stop();
    }
public:
    int start() noexcept;
    void stop() const noexcept;
private:
    int sendMessage(const std::string& message) const noexcept;
    Message* receiveMessage() const noexcept;
    static void printInputPrompt() noexcept;
    int inputHandler();
    int handleConnection() noexcept;

private:
    const std::string hostname;
    in_port_t port;

    SOCKET socket;
};
#endif