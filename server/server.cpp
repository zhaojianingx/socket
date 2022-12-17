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
    int client_socket[30];
    int max_clients = 30;
    char *buf = new char[4096];

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
            std::cout << "New connection, ip is : "  << inet_ntoa(hint.sin_addr) << ", port :" << ntohs(hint.sin_port) << std::endl;

              
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
                byte_received = recv(sd, buf, 4096, 0);
                //Check if it was for closing , and also read the incoming message
                if ((byte_received == 0)) {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&hint , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(hint.sin_addr) , ntohs(hint.sin_port));
                    
                    //Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket[i] = 0;
                }
                //Echo back the message that came in
                else
                {
                    
                    send(sd, buf, byte_received + 1, 0);
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
void TcpServer::Query(int client_socket) {

}

// settle the order
void TcpServer::Order(int food_id, int nums) {
    for (std::pair<std::string, int> food_item : food_list[food_id]) {
        food_item.second += nums;
    }
}