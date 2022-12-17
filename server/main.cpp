#include "server.h"

int main(int argc, char *argv[]) {
    int port = (std::string(argv[1]) == "A") ? 55000 : 54000;
    TcpServer tcp_server(port, 100, "");
    tcp_server.Run();
    return 0;
}