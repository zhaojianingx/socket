#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>

int main() {
    char ch;
    while (true) {
        std::cout << "Select the Server you want to connect ? (A or a for S1, B or b for S2)" << std::endl;
        std::cin >> ch;
        if (ch == 'a' || ch == 'A' || ch == 'B' || ch == 'b') break;
    }
    int server_port_ = (ch == 'A' || ch == 'a') ? 55000 : 54000;
    std::string server_ip_ = "127.0.0.1";
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

    std::string server_name = (ch == 'A' || ch == 'a') ? "S1" : "S2";
    std::cout << "Successfully connect to server " + server_name << std::endl;

    // Do-while loop to send and receive data
    char buf[1024];
    std::string user_input;
    std::cin.get();
    do {
        std::cout << "请输入您的操作，输入list查看食品列表，输入insert进行添加，输入update进行修改，输入delete进行删除，输入#退出" << std::endl;
        // Prompt the user for some text
        std::cin >> user_input;
        if (user_input.size() > 0) {
            if (user_input == "insert") {
                send(sock, user_input.c_str(), user_input.size() + 1, 0);
                std::cout << "请输入您要添加的食品id、名称和数量" << std::endl;
                std::string name;
                int id = 0, num = 0;
                std::cin >> id >> name >> num;
                user_input = std::to_string(id) + "," + name + "," + std::to_string(num);
                if (id > 0 && name != "" && num > 0) {
                    int send_result = send(sock, user_input.c_str(), user_input.size() + 1, 0);
                    if (send_result != -1) {
                        // Wait for response
                        memset(buf, 0, sizeof(buf));
                        int byte_received = recv(sock, buf, 1024, 0);
                        if (byte_received > 0) {
                            // Echo response to console
                            std::cout << std::string(buf, 0, byte_received) << std::endl;
                        }
                    }
                } else {
                    std::cout << "您的输入有误，请重新输入" << std::endl;
                }
            } else if (user_input == "list") {
                // Send the text
                int send_result = send(sock, user_input.c_str(), user_input.size() + 1, 0);
                if (send_result != -1) {
                    // Wait for response
                    memset(buf, 0, sizeof(buf));
                    int byte_received = recv(sock, buf, 1024, 0);
                    if (byte_received > 0) {
                        // Echo response to console
                        std::cout << std::string(buf, 0, byte_received) << std::endl;
                    }
                }
            } else if (user_input == "update") {
                send(sock, user_input.c_str(), user_input.size() + 1, 0);
                std::cout << "请输入您要更新的食品id和数量" << std::endl;
                int id = 0, num = 0;
                std::string name;
                std::cin >>id >> name >> num;
                user_input = std::to_string(id) + "," +  name + "," + std::to_string(num);
                if (id > 0 && name != "" && num > 0) {
                    int send_result = send(sock, user_input.c_str(), user_input.size() + 1, 0);
                    if (send_result != -1) {
                        // Wait for response
                        memset(buf, 0, sizeof(buf));
                        int byte_received = recv(sock, buf, 1024, 0);
                        if (byte_received > 0) {
                            // Echo response to console
                            std::cout << std::string(buf, 0, byte_received) << std::endl;
                        }
                    }
                } else {
                    std::cout << "您的输入有误，请重新输入" << std::endl;
                }
            } else if (user_input == "delete") {
                send(sock, user_input.c_str(), user_input.size() + 1, 0);
                std::cout << "请输入您要删除的食品id" << std::endl;
                int id = 0;
                std::cin >> id;
                user_input = std::to_string(id);
                if (id > 0) {
                    int send_result = send(sock, user_input.c_str(), user_input.size() + 1, 0);
                    if (send_result != -1) {
                        // Wait for response
                        memset(buf, 0, sizeof(buf));
                        int byte_received = recv(sock, buf, 1024, 0);
                        if (byte_received > 0) {
                            // Echo response to console
                            std::cout << std::string(buf, 0, byte_received) << std::endl;
                        }
                    }
                } else {
                    std::cout << "您的输入有误，请重新输入" << std::endl;
                }
            } else if (user_input == "#") {
                break;
            } else {
                std::cout << "您的输入有误，请重新输入" << std::endl;
            }
            
        }
    } while (user_input.size() > 0);

    // Close down
    close(sock);



    return 0;
}