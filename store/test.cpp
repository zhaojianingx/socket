#include <iostream>
#include <string>
int main() {
    int id = 100;
    std::string user_input = std::to_string(id);
    std::cout << user_input.size() << std::endl;
}