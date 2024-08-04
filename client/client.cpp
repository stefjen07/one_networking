#include "client.h"
#include "../message_length.h"
#include "../connection.h"
#include <thread>

int Client::start() noexcept {
    INFO("Starting the client");

    addrinfo* conn_addr = resolveConnectionAddress(hostname, port);
    if (!conn_addr){
        return -1;
    }

    socket = createSocket(conn_addr);
    if (!IS_VALID_SOCKET(socket)){
        return -1;
    }

    INFO("Connecting to remote host (" << hostname << ":" << port << ")");
    if (connect(socket, conn_addr->ai_addr, conn_addr->ai_addrlen) == -1){
        ERROR(-1, "Failed to connect to the remote host: connect(): " << std::system_category().message(GET_SOCKET_ERRNO()));
    }
    INFO("Successfully connected to " << hostname << ":" << port);

    freeaddrinfo(conn_addr);

    return handleConnection();
}

void Client::stop() const noexcept {
    INFO("Disconnecting from the remote server.");
    CLOSE_SOCKET(socket);
    INFO("Disconnected.");
}

int Client::sendMessage(const std::string &message) const noexcept {
    ::sendMessage(socket, message.data(), message.size());
}

Message* Client::receiveMessage() const noexcept {
    ::receiveMessage(socket, nullptr);
}

void Client::printInputPrompt() noexcept {
    std::cin.clear();
    std::cout << " >>> ";
    std::cout.flush();
}

int Client::inputHandler(){
    while (true){
        char msg_buff[MESSAGE_MAX_SIZE];
        printInputPrompt();

        std::fgets(msg_buff, MESSAGE_MAX_SIZE, stdin);
        std::string message_str(msg_buff);
        message_str.pop_back();
        if (sendMessage(message_str) == -1){
            std::exit(1);
        }
        memset(msg_buff, 0x00, MESSAGE_MAX_SIZE);
    }
}

int Client::handleConnection() noexcept {
    std::thread input_worker_thread(&Client::inputHandler, this);
    input_worker_thread.detach();
    while (true){
        Message* recv_bytes = receiveMessage();
        if (recv_bytes == nullptr) {
            std::cerr << "[Error] Failed to receive data from the remote host: recv(): " << std::system_category().message(GET_SOCKET_ERRNO()) << std::endl;
            std::exit(1);
        }
        std::cout << std::string(recv_bytes->content, recv_bytes->length) << '\n';

        printInputPrompt();
    }
}