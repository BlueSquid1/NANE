#include <iostream> //cout
#include <thread> //thread

#include "network/Logger_server.h"

enum ServerType
{
    StandardOut,
    StandardError
};

void thread_main(ServerType serverType, int port)
{
    Logger_server stdOutServer(port);
    int stdFd = stdOutServer.WaitForConnection();
    if(stdFd == -1)
    {
        std::cout << "failed to read client on port: " << port << std::endl;
    }

    std::cout << "--------" << std::endl;

    while(stdOutServer.getConnected())
    {
        std::string outMsg = stdOutServer.ReceiveMsg();
        if(serverType == StandardOut)
        {
            std::cout << outMsg;
        }
        else
        {
            std::cerr << outMsg;
        }
    }
}


int main(int argc, char *argv[])
{
    int stdOutPort = 8067;
    std::thread stdOutThread(thread_main, StandardOut, stdOutPort);

    int stdErrPort = 8068;
    std::thread stdErrThread(thread_main, StandardError, stdErrPort);

    stdOutThread.join();
    stdErrThread.join();
    return 0;
}