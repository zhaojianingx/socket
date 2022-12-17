#include "server.h"

int main() {
    TcpServer tcp_server(54000, 100, "");
    tcp_server.Run();
    return 0;
}