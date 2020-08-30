#include "RomLoader.h"
#include <sstream>

std::string RomLoader::ListFilesInPath(const std::string& fileSystemPath )
{
    std::stringstream fileStringStream;
    fileStringStream << "hello" << std::endl;
    fileStringStream << "world" << std::endl;
    return fileStringStream.str();
}

std::string RomLoader::GetFilesAtCurrentPath()
{
    return this->ListFilesInPath(this->currentFilesystemPath);
}