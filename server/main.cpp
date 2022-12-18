#include "server.h"

int main(int argc, char *argv[]) {
    int port = (std::string(argv[1]) == "A") ? 55000 : 54000;
    //std::string file_name = (std::string(argv[1]) == "A") ? "A_data" : "B_data";
    //TcpServer tcp_server = TcpServer::Deserialize(file_name);
    TcpServer tcp_server(port, 30, "");
    tcp_server.Run();
    return 0;
}