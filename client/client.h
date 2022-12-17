#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>


class TcpClient {
public:
    TcpClient(int server_port, std::string server_ip);
    TcpClient(std::string server_ip, char choice_server);
    virtual ~TcpClient();
public: 
    int Run();
private:
    virtual void ClientFunction(int connected_socket);
private:
    int server_port_;
    std::string server_ip_;
    char choice_server_; // 选择服务器
};
