#include "RomLoader.h"

std::vector<std::string> RomLoader::ListFilesInPath(const std::string& fileSystemPath )
{
    std::vector<std::string> fileList;
    fileList.push_back("hello");
    fileList.push_back("world");
    return fileList;
}

std::string RomLoader::GetCurrentFilesystemPath() const
{
    return this->currentFilesystemPath;
}

void RomLoader::SetCurrentFilesystemPath(const std::string& path)
{
    this->currentFilesystemPath = path;
}