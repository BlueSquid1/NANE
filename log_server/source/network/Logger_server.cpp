#include "Logger_server.h"

Logger_server::Logger_server(int serverListeningPort)
{
    this->listeningPort = serverListeningPort;

    std::cout << "creating socket" << std::endl;
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(this->serverSocket == -1)
    {
        std::cout << "Failed to create socket" << std::endl;
        return;
    }
    std::cout << "succesfully created socket" << std::endl;

    
    std::cout << "setting socket options " << std::endl;
    int opt = 1;
    int sockOp = setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    if(sockOp != 0)
    {
        std::cout << "failed to options on socket: setsockopt " << sockOp << std::endl;
    }
    std::cout << "setting socket options successful" << std::endl;

    std::cout << "starting to bind to port: " << this->listeningPort << std::endl;
    struct sockaddr_in srv_addr;
    bzero(&srv_addr, sizeof srv_addr);
    int addrlen = sizeof(srv_addr); 
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(this->listeningPort);
    int ret = bind(this->serverSocket, (struct sockaddr *) &srv_addr, addrlen);
    if(ret != 0)
    {
        std::cout << "Failed to bind to port:" << this->listeningPort << "bind() " << ret << std::endl;
        close(this->serverSocket);
        return;
    }
    std::cout << "successfully bind on port: " << this->listeningPort << std::endl;

    std::cout << "start to listen on port: " << this->listeningPort << std::endl;
    int listenRet = listen(this->serverSocket, 3);
    if(listenRet != 0)
    {
        std::cout << "Failed to listen to socket: " << this->serverSocket << ". listen() " << listenRet << std::endl;
        close(this->serverSocket);
        return;
    }

    std::cout << "successfully started listening on port: " << this->serverSocket << std::endl;
}

int Logger_server::WaitForConnection()
{
    struct sockaddr_in srv_addr;
    bzero(&srv_addr, sizeof srv_addr);
    int addrlen = sizeof(srv_addr);

    std::cout << "waiting to accept" << std::endl;
    this->clientSocket = accept(this->serverSocket, (struct sockaddr *) &srv_addr, (socklen_t*) &addrlen);
    if (this->clientSocket == -1)
    {
        std::cout << "ERROR on accept" << std::endl;
        return -1;
    }
    std::cout << "accepted client on socket: " << this->clientSocket << std::endl;

    return this->clientSocket;
}


std::string Logger_server::ReceiveMsg()
{
    const int bufferLen = 256;
    char coutBuffer[bufferLen];
    int bytesActuallyRead = recv(this->clientSocket, coutBuffer, bufferLen, 0);
    std::string bufferString(coutBuffer, bytesActuallyRead);
    return bufferString;
}

Logger_server::~Logger_server()
{
    close(this->serverSocket);
    close(this->clientSocket);
}