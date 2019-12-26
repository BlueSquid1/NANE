#ifndef NETWORK_LOGGER
#define NETWORK_LOGGER

#include <iostream> //std::string std::ostream
#include <strings.h> //bzero()
#include <sys/socket.h> //for socket()
#include <arpa/inet.h> //for struct sockaddr_in
#include <unistd.h> //close()

#ifdef __SWITCH__
#include <switch.h> //socketInitializeDefault()
#endif
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
    static bool Initalize(const std::string & serverIp, int stdOutFD = STDOUT_FILENO, int stdErrFD = STDERR_FILENO);
    static void Close();
};

#endif