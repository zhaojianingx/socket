#include "client.h"

int main() {
    char ch;
    while (true) {
        std::cout << "Select the Server you want to connect ? (A or a for S1, B or b for S2)" << std::endl;
        std::cin >> ch;
        if (ch == 'a' || ch == 'A' || ch == 'B' || ch == 'b') break;
    }
    
    TcpClient client("127.0.0.1", ch);
    client.Run();
    return 0;
}