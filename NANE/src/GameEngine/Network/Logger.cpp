#include "Logger.h"

std::string Logger::serverIp = std::string();
Logger * Logger::stdOutLogger = NULL;
Logger * Logger::stdErrLogger = NULL;

bool Logger::Initalize(std::string serverIpAddress, int stdOutFD, int stdErrFD)
{
    if( serverIpAddress.empty() )
    {
        std::cout << "no server ip address provided. sending logs over network has been disabled" << std::endl;
        return true;
    }
    std::cout << "sending logs over network to ip: " << serverIpAddress << std::endl;

    #ifdef __SWITCH__
    Result socketsInit = socketInitializeDefault();
    if (R_FAILED(socketsInit))
    {
        return false;
    }
    #endif

    Logger::serverIp = serverIpAddress;
    int stdOutPort = 8067;
    int stdErrPort = 8068;
    Logger::stdOutLogger = new Logger(stdOutFD, stdOutPort);
    Logger::stdErrLogger = new Logger(stdErrFD, stdErrPort);
    return true;
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
        std::cout << "closing socket: " << this->serverSocket << std::endl;
        close(this->serverSocket);
    }
}

void Logger::Close()
{
    delete Logger::stdOutLogger;
    delete Logger::stdErrLogger;

    Logger::stdOutLogger = NULL;
    Logger::stdErrLogger = NULL;
    
    #ifdef SWITCH
    socketExit();
    #endif
}