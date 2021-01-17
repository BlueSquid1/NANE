#pragma once

#include <iostream> //std::string std::ostream
#include <strings.h> //bzero()
#include <sys/socket.h> //for socket()
#include <arpa/inet.h> //for struct sockaddr_in
#include <unistd.h> //close()

#ifdef __SWITCH__
#include <switch.h> //socketInitializeDefault()
#endif

/**
 * This class is responsible for sending messages written to std::cout and std::cerr and sending them to a network server.
 */
class Logger
{
    private:
    //log server IP address
    static std::string serverIp;
    //instances for the cout and cerr logging
    static Logger * stdOutLogger;
    static Logger * stdErrLogger;

    //network socket
    int serverSocket = -1;
    
    private:
    //private constructor
    Logger(int fileDescriptor, int serverPort);
    //deconstructor
    ~Logger();

    public:
    /**
     * initalize network logger. This method should be called once before any other method.
     * @param serverIp the logging server ip. If set to empty string then network logging is disabled.
     * @param stdOutFD the file descripter to send standard output messages. Defaults to std::cout.
     * @param stdErrFD the file descripter to send standard error messages. Defaults to std::cerr.
     * @return false if a fatal error has occured.
     */
    static bool Initalize(const std::string & serverIp, int stdOutFD = STDOUT_FILENO, int stdErrFD = STDERR_FILENO);

    /**
     * Closes the network logger.
     */
    static void Close();
};