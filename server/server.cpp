#include "server.h"

TcpServer::TcpServer(int server_port, int listen_queue_length, std::string ip_address) {
    server_port_ = server_port;
    listen_queue_length_ = listen_queue_length;
    ip_address_ = ip_address.empty() ? "" : ip_address;
}

int TcpServer::Run() {
    // Create a socket
    int listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listening_socket) {
        std::cerr << "Can't create the listening socket! Quitting" << std::endl;
        return -1;
    }

    // Bind the ip address and port to a socket
    sockaddr_in hint;
    memset(&hint, 0, sizeof(sockaddr_in));
    hint.sin_family = AF_INET;
    hint.sin_port = htons(server_port_);
    if (ip_address_.empty()) {
        hint.sin_addr.s_addr = htonl(INADDR_ANY);
    } else if (inet_pton(AF_INET, ip_address_.c_str(), &hint.sin_addr) != 1) {
        std::cerr << "inet_pton error" << std::endl;
        close(listening_socket);
        return -1;
    }
    
    if (bind(listening_socket, (sockaddr *)&hint, sizeof(sockaddr_in)) == -1) {
        std::cerr << "bind error" << std::endl;
        close(listening_socket);
        return -1;
    }
    // Listen
    if(listen(listening_socket, listen_queue_length_) == -1) {
        std::cerr << "listen error" << std::endl;
        close(listening_socket);
        return -1;
    }

    while (true) {
        // Wait for a connection
        sockaddr_in client;
        socklen_t client_size = sizeof(sockaddr_in);

        int client_socket = accept(listening_socket, (sockaddr *)&client, &client_size);
        if (client_socket == -1) {
            std::cerr<< "Can't accept the client socket! Quitting" << std::endl;
            close(listening_socket);
            return -1;
        }

        std::string host; // Client's remote name
        int service; // Service (i.e. port) the client is connect on

        sockaddr_in addr;
        socklen_t addr_size = sizeof(sockaddr_in);
        if (getpeername(client_socket, (sockaddr *)&addr, &addr_size) == 0) {
            host = inet_ntoa(addr.sin_addr);
            service = ntohs(addr.sin_port);
            std:: cout << host << " connected on port " << service << std::endl;
        }

        // While loop: accept and echo message back to client
        ServerFunction(client_socket, listening_socket);

        // Close the socket
        close(client_socket);
    }

    // Close listening socket
    close(listening_socket);
    
    return 0;
}

void TcpServer::ServerFunction(int client_socket, int listening_socket) {
    char buf[4096];

    while (true) {
        memset(buf, 0, sizeof(buf));

        // Wait for client to send data
        int byte_received = recv(client_socket, buf, 4096, 0);
        if (byte_received == -1) {
            std::cerr << "Error in recv(). Quitting" << std::endl;
            break;
        }
        if (byte_received == 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        // Echo message back to client
        send(client_socket, buf, byte_received + 1, 0);
    }
}

void TcpServer::Query(int client_socket) {

}

void TcpServer::Order(int food_id) {
    //food_list[food_id]
}