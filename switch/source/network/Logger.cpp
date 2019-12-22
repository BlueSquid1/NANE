#include "Logger.h"

std::string Logger::serverIp = std::string();
Logger * Logger::stdOutLogger = NULL;
Logger * Logger::stdErrLogger = NULL;

//const std::string Logger::SERVER_IP = "10.48.119.4";

void Logger::Initalize(std::string serverIpAddress, int stdOutFD, int stdErrFD)
{
    Result socketsInit = socketInitializeDefault();
    if (R_FAILED(socketsInit))
    {
        return;
    }

    Logger::serverIp = serverIpAddress;
    int stdOutPort = 8067;
    int stdErrPort = 8068;
    Logger::stdOutLogger = new Logger(stdOutFD, stdOutPort);
    Logger::stdErrLogger = new Logger(stdErrFD, stdErrPort);
}

Logger::Logger(int fileDescriptor, int serverPort)
{
    //try to create a socket
    int protocol = 0;
    this->serverSocket = socket(PF_INET, SOCK_STREAM, protocol);
    if(this->serverSocket == -1)
    {
        //Failed to create socket
        return;
    }

    struct sockaddr_in srv_addr;
    bzero(&srv_addr, sizeof srv_addr);
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(serverPort);
    inet_pton(AF_INET, Logger::serverIp.c_str(), &srv_addr.sin_addr);

    int connectRet = connect(this->serverSocket, (struct sockaddr *) &srv_addr, sizeof(srv_addr));
    if (connectRet != 0) {
        //Failed to connect to server
        close(this->serverSocket);
        return;
    }

    // redirect output to the socket
    dup2(this->serverSocket, fileDescriptor);
}

Logger::~Logger()
{
    if(this->serverSocket != -1)
    {
        close(this->serverSocket);
    }
}

void Logger::Close()
{
    delete Logger::stdOutLogger;
    delete Logger::stdErrLogger;

    Logger::stdOutLogger = NULL;
    Logger::stdErrLogger = NULL;

    socketExit();
}