#include "server.h"

TcpServer::TcpServer(int server_port, int listen_queue_length, std::string ip_address) {
    server_port_ = server_port;
    listen_queue_length_ = listen_queue_length;
    ip_address_ = ip_address.empty() ? "" : ip_address;
    
}

int TcpServer::Run() {
    //set of socket descriptors
    fd_set readfds;
    int new_socket;
    int opt = 1;
    int sd;
    int max_sd;
    int activity;
    int client_socket[3];
    int max_clients = 3;
    char *buf = new char[128];

    //initialise all client_socket[] to 0 so not checked
    for (int i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    // Create a socket
    int listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listening_socket) {
        std::cerr << "Can't create the listening socket! Quitting" << std::endl;
        return -1;
    }

    //set master socket to allow multiple connections , this is just a good habit, it will work without this
    if(setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        std::cerr << "Error on sotsockopt" << std::endl;
        return -1;
    }

    // Bind the ip address and port to a socket
    sockaddr_in hint;
    int addrlen = sizeof(hint);
    memset(&hint, 0, sizeof(sockaddr_in));
    hint.sin_family = AF_INET;
    hint.sin_port = htons(server_port_);
    if (ip_address_.empty()) {
        hint.sin_addr.s_addr = htonl(INADDR_ANY);
    } else if (inet_pton(AF_INET, ip_address_.c_str(), &hint.sin_addr) != 1) {
        std::cerr << "inet_pton error" << std::endl;
        close(listening_socket);
        return -1;
    }
    
    if (bind(listening_socket, (sockaddr *)&hint, sizeof(sockaddr_in)) == -1) {
        std::cerr << "bind error" << std::endl;
        close(listening_socket);
        return -1;
    }
    // Listen
    if(listen(listening_socket, listen_queue_length_) == -1) {
        std::cerr << "listen error" << std::endl;
        close(listening_socket);
        return -1;
    }
    std::string msg = (server_port_ == 55000) ? "Sever S1 is up" : "Server S2 is up";
    std::cout << msg << std::endl;
    std::cout << "Waiting for connections" << std::endl;

    while (true) {
         //clear the socket set
        FD_ZERO(&readfds);
  
        //add master socket to set
        FD_SET(listening_socket, &readfds);
        max_sd = listening_socket;
         
        //add child sockets to set
        for (int i = 0 ; i < max_clients ; i++) {
            //socket descriptor
            sd = client_socket[i];
             
            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET(sd , &readfds);
             
            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }
        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select(max_sd + 1 , &readfds , NULL , NULL , NULL);
    
        if ((activity < 0) && (errno!=EINTR)) {
            std::cout<< "select error" << std::endl;
        }
        int byte_received;
        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(listening_socket, &readfds)) 
        {
            if ((new_socket = accept(listening_socket, (struct sockaddr *)&hint, (socklen_t*)&addrlen)) < 0)
            {
                std::cout << "error happen" << std::endl;
                return -1;
            }
          
            //inform user of socket number - used in send and receive commands
            std::cout << "New connection, ip is : "  << inet_ntoa(hint.sin_addr) << ", port: " << ntohs(hint.sin_port) << std::endl;

              
            //add new socket to array of sockets
            for (int i = 0; i < max_clients; i++) 
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    //printf("Adding to list of sockets as %d\n" , i);
                    std::cout << "Adding to list of sockets as " << i << std::endl; 
                    break;
                }
            }
        }

        //else its some IO operation on some other socket :)
        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];
              
            if (FD_ISSET(sd , &readfds)) {
                byte_received = recv(sd, buf, 128, 0);
                //Check if it was for closing , and also read the incoming message
                if ((byte_received == 0)) {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&hint , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip is %s , port: %d \n" , inet_ntoa(hint.sin_addr) , ntohs(hint.sin_port));
                    
                    //Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket[i] = 0;
                }
                //Echo back the message that came in
                else
                {
                    if (strcmp(buf, "list") == 0) {
                        Query(buf, sd);
                    } else if (strcmp(buf, "order") == 0) {
                        Order(buf, sd);
                    } else if (strcmp(buf, "insert") == 0) {
                        Insert(buf, sd);
                    } else if (strcmp(buf, "update") == 0) {
                        Update(buf, sd);
                    } else if (strcmp(buf, "delete") == 0) {
                        Delete(buf, sd);
                    } else if (strcmp(buf, "#") == 0) {
                        KeepAlive(buf, sd);
                    }
                }
                
            }
        }
    }

    delete[] buf;
    // Close listening socket
    close(listening_socket);
    return 0;
}

// Query all the food
void TcpServer::Query(char * buf, int socket) {
    std::string str("id\tname\tnum\n");
    for (int i = 0; i < food_list.size(); i++) {
        Food food = food_list[i];
        if (i == food_list.size() - 1) {
            str += std::to_string(food.food_id)  + "\t" +  food.food_name + "\t" + std::to_string(food.food_num);
        } else {
            str += std::to_string(food.food_id)  + "\t" +  food.food_name + "\t" + std::to_string(food.food_num) + "\n";
        }
    }
    send(socket, str.c_str(), str.size() + 1, 0);
}

// settle the order
void TcpServer::Order(char * buf, int socket) {
    int byte_received = recv(socket, buf, 128, 0);
    std::string delimiter_char = ",";
    size_t pos = 0;
    std::string s = std::string(buf);
    int id, num;
    if ((pos = s.find(delimiter_char)) != std::string::npos) {
        id = stoi(s.substr(0, pos));
        s.erase(0, pos + delimiter_char.length());
    }
    num = stoi(s);
    std::string success = "订单结算成功";
    std::string error = "您输入的食品信息有误，请重新输入";
    bool flag = false;
    for (Food &food : food_list) {
        if (id == food.food_id) {
            flag = true;
            if (food.food_num >= num) {
                food.food_num -= num;
                send(socket, success.c_str(), success.size() + 1, 0);
                std::string file_name = (server_port_ == 55000) ? "A_data" : "B_data";
                Serialize(file_name);
            } else {
                send(socket, error.c_str(), error.size() + 1, 0);
            }
        }
    }
    if (!flag) send(socket, error.c_str(), error.size() + 1, 0);
}

void TcpServer::Insert(char * buf, int socket) {
    int byte_received = recv(socket, buf, 128, 0);
    std::string delimiter_char = ",";
    size_t pos = 0;
    std::string s = std::string(buf);
    std::string name;
    int id, num;
    if ((pos = s.find(delimiter_char)) != std::string::npos) {
        id = stoi(s.substr(0, pos));
        s.erase(0, pos + delimiter_char.length());
    }
    if ((pos = s.find(delimiter_char)) != std::string::npos) {
        name = s.substr(0, pos);
        s.erase(0, pos + delimiter_char.length());
    }
    num = stoi(s);
    std::string success = "添加食品成功";
    std::string error = "您输入的食品信息有误，请重新输入";
    if (name.size() > 0 && name.size() < 20 && num > 0) {
        food_list.push_back(Food{id, name, num});
        send(socket, success.c_str(), success.size() + 1, 0);
        std::string file_name = (server_port_ == 55000) ? "A_data" : "B_data";
        Serialize(file_name);
    } else {
        send(socket, error.c_str(), error.size() + 1, 0);
    }
    
}

void TcpServer::Update(char * buf, int socket) {
    int byte_received = recv(socket, buf, 128, 0);
    std::string delimiter_char = ",";
    size_t pos = 0;
    std::string s = std::string(buf);
    std::string name;
    int id, num;
    if ((pos = s.find(delimiter_char)) != std::string::npos) {
        id = stoi(s.substr(0, pos));
        s.erase(0, pos + delimiter_char.length());
    }
    if ((pos = s.find(delimiter_char)) != std::string::npos) {
        name = s.substr(0, pos);
        s.erase(0, pos + delimiter_char.length());
    }
    num = stoi(s);
    std::string success = "修改食品成功";
    std::string error = "您输入的食品信息有误，请重新输入";
    if (name.size() > 0 && name.size() < 20 && num > 0) {
        for (Food &food : food_list) {
            if(id == food.food_id) {
                food.food_name = name;
                food.food_num = num;
                send(socket, success.c_str(), success.size() + 1, 0);
                std::string file_name = (server_port_ == 55000) ? "A_data" : "B_data";
                Serialize(file_name);
            }
        }
    } else {
        send(socket, error.c_str(), error.size() + 1, 0);
    }
}

void TcpServer::Delete(char * buf, int socket) {
    int byte_received = recv(socket, buf, 128, 0);
    std::string delimiter_char = ",";
    size_t pos = 0;
    int id = std::stoi(std::string(buf, 0, byte_received));
    std::cout << "buyte" << id << std::endl;
    std::string success = "删除食品成功";
    std::string error = "您输入的食品信息有误，请重新输入";
    bool flag = false;
    int index = 0;
    for (int i = 0; i < food_list.size(); ++i) {
        if (id == food_list[i].food_id) {
            flag = true;
            index = i;
        }
    }
    food_list.erase(food_list.begin() + index);
    send(socket, success.c_str(), success.size() + 1, 0);
    std::string file_name = (server_port_ == 55000) ? "A_data" : "B_data";
    Serialize(file_name);
    if (!flag) send(socket, error.c_str(), error.size() + 1, 0);
}

void TcpServer::KeepAlive(char * buf, int socket) {
    
}

bool TcpServer::Serialize(std::string file_name) {
    std::ofstream *os = new std::ofstream(file_name);
    for (Food food : food_list) {
        os -> write((char*)&food.food_id, sizeof(int));
        int len = food.food_name.size();
        char chars[20];
        for (int i = 0; i < len; i++) {
            chars[i] = food.food_name[i];
        }
        chars[len] = '\0';
        memset(chars + sizeof(len + 1), 0, 20 - sizeof(len + 1));
        os -> write(chars, 20);
        os -> write((char*)&food.food_num, sizeof(int));
    }
    os -> close();
    delete os;
    return true;
}

bool TcpServer::Deserialize(std::string file_name, std::vector<Food> &vec) {
    char *buf = new char[20];
    Food food;
    std::ifstream is(file_name);
    // int id, num;
    // std::string name;
    
    int begin = is.tellg();
    is.seekg (0, std::ios::end);
    int end = is.tellg();
    int fsize = (end-begin);
    //std::cout << fsize << std::endl;

    is = std::ifstream(file_name);
    while(fsize > 0) {
        is.read(buf, 4);
        int *tmp = (int*)buf;
        food.food_id = *tmp;
        is.read(buf, 20);
        food.food_name = std::string(buf);
        is.read(buf, 4);
        tmp = (int*)buf;
        food.food_num = *tmp;
        //std::cout << id << "\t" <<name << "\t" <<num << std::endl;
        fsize -= 28;
        vec.push_back(food);
    }
    delete[] buf;
    return true;
}
