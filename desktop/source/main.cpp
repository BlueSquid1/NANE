#include "network/Logger_server.h"


int main(int argc, char *argv[])
{
    int stdOutPort = 8067;
    int stdErrPort = 8068;
    Logger_server stdOutServer(stdOutPort);
    Logger_server stdErrServer(stdErrPort);

    int stdOutFd = stdOutServer.WaitForConnection();
    if(stdOutFd == -1)
    {
        std::cout << "failed to read client standard output" << std::endl;
    }
    int stdErrFd = stdErrServer.WaitForConnection();
    if(stdErrFd == -1)
    {
        std::cout << "failed to read client standard errors" << std::endl;
    }

    while(true)
    {
        std::string outMsg = stdOutServer.ReceiveMsg();
        std::cout << outMsg;

        std::string errMsg = stdErrServer.ReceiveMsg();
        std::cerr << errMsg;
    }
}