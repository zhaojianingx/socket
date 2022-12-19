#include "server.h"

int main(int argc, char *argv[]) {
    int port = (std::string(argv[1]) == "A") ? 55000 : 54000;
    std::string file_name = (std::string(argv[1]) == "A") ? "A_data" : "B_data";
    int heart_port = (std::string(argv[1]) == "B") ? 55000 : 54000;
    std::string heart_file_name = (std::string(argv[1]) == "B") ? "A_data" : "B_data";
    pid_t c_pid = fork();
  
    if (c_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (c_pid > 0) {
        //  wait(nullptr);
        TcpServer tcp_server(port, 30, "");
        TcpServer::Deserialize(file_name, tcp_server.food_list);
        tcp_server.Run();
    }
    else {
        TcpServer::HeartBeat(heart_port, heart_file_name);
    }
   
    return 0;
}