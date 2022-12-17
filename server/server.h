#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <string>
#include <map>

class TcpServer {
public:
    TcpServer(int server_port, int listen_queue_length, std::string ip_address);
    virtual ~TcpServer(){};
public:
    int Run();
    void Query(int client_socket);
    void Order(int food_id, int nums);
private:
    virtual void ServerFunction(int client_socket, int listening_socket);
private:
    int server_port_;
    int listen_queue_length_;
    std::string ip_address_;
    std::map<int, std::map<std::string, int>> food_list;
};

#endif /* !SERVER_H */