#include "client.h"

TcpClient::TcpClient(int server_port, std::string server_ip) {
    server_port_ = server_port;
    server_ip_ = server_ip;
}

TcpClient::TcpClient(std::string server_ip, char choice_server) {
    server_ip_ = server_ip;
    choice_server_ = choice_server;
    server_port_ = (choice_server_ == 'A' || choice_server_ == 'a') ? 55000 : 54000;
}

TcpClient::~TcpClient() {

}

int TcpClient::Run() {
    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Can't create a new socket" << std::endl;
        return -1;
    }

    // Fill in a hint structure
    sockaddr_in hint;
    memset(&hint, 0, sizeof(sockaddr_in));
    hint.sin_family = AF_INET;
    hint.sin_port = htons(server_port_);
    if (inet_pton(AF_INET, server_ip_.c_str(), &hint.sin_addr) != 1) {
        std::cerr << "inet_pton error" << std::endl;
        close(sock);
        return -1;
    }
    
    // Connect to server
    if (connect(sock, (sockaddr *)&hint, sizeof(hint)) == -1) {
        std::cerr << "Can't coonect to server" << std::endl;
        close(sock);
        return -1;
    }

    std::string server_name = (choice_server_ == 'A' || choice_server_ == 'a') ? "S1" : "S2";
    std::cout << "Successfully connected to server " + server_name << std::endl;

    // Do-while loop to send and receive data
    ClientFunction(sock);

    // Close down
    close(sock);

    return 0;
}

void TcpClient::ClientFunction(int connected_socket) {
    char buf[4096];
    std::string user_input;
    std::cin.get();
    do {
        // Prompt the user for some text
        getline(std::cin, user_input);
        if (user_input.size() > 0) {
            // Send the text
            int send_result = send(connected_socket, user_input.c_str(), user_input.size() + 1, 0);
            if (send_result != -1) {
                // Wait for response
                memset(buf, 0, sizeof(buf));
                int byte_received = recv(connected_socket, buf, 4096, 0);
                if (byte_received > 0) {
                    // Echo response to console
                    std::cout << "Server: " << std::string(buf, 0, byte_received) << std::endl;
                }
            }
        }
    } while (user_input.size() > 0);
}