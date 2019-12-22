#ifndef NETWORK_LOGGER
#define NETWORK_LOGGER

#include <iostream> //std::string
#include <strings.h> //bzero()
#include <sys/socket.h> //for socket()
#include <arpa/inet.h> //for struct sockaddr_in
#include <unistd.h> //close()

#include <switch.h> //socketInitializeDefault()

class Logger
{
    private:
    static std::string serverIp;
    static Logger * stdOutLogger;
    static Logger * stdErrLogger;

    int serverPort = -1;
    int serverSocket = -1;
    
    private:
    //private constructor
    Logger(int fileDescriptor, int serverPort);
    ~Logger();

    public:
    static void Initalize(std::string serverIp, int stdOutFD = STDOUT_FILENO, int stdErrFD = STDERR_FILENO);
    static void Close();
};

#endif