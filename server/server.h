#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <sys/time.h>
#include <sys/types.h>
#include <fstream>
#include <memory>

struct Food {
    int food_id;
    std::string food_name;
    int food_num;
};

class TcpServer {
public:
    TcpServer(int server_port, int listen_queue_length, std::string ip_address);
    virtual ~TcpServer(){};
public:
    int Run();
    //static TcpServer &Deserialize(std::string file_name);
private:
    void Query(char * buf, int socket);
    void Order(char * buf, int socket);
    void Insert(char * buf, int socket);
    void Update(char * buf, int socket);
    void Delete(char * buf, int socket);
    void KeepAlive(char * buf, int socket);
    //bool Serialize(std::string file_name);
    
private:
    int server_port_;
    int listen_queue_length_;
    std::string ip_address_;
    std::vector<Food> food_list;
};


#endif /* !SERVER_H */