#ifndef NETWORK_LOGGER_SERVER
#define NETWORK_LOGGER_SERVER

#include <iostream> //std::string
#include <strings.h> //bzero()
#include <sys/socket.h> //for socket()
#include <arpa/inet.h> //for struct sockaddr_in
#include <unistd.h> //close()

class Logger_server
{
    private:
    int listeningPort = -1;
    int serverSocket = -1;
    int clientSocket = -1;

    public:
    //constructor
    Logger_server(int listeningPort);
    ~Logger_server();

    //returns the file descriptor for the newly connected client
    int WaitForConnection();
    
    std::string ReceiveMsg();
};

#endif