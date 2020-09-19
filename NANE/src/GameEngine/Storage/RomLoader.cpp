#include "RomLoader.h"

#include <iostream> //std::cerr

#include <dirent.h> //posix library - readdir()
#include <sys/stat.h> //posix library - stat()
#include <SDL_filesystem.h> //SDL_GetBasePath()

std::string RomLoader::GetDefaultPath()
{
    char * sdlDefaultPath = SDL_GetBasePath();
    std::string defaultPath(sdlDefaultPath);

    // GetBasePath adds a '/' or '\\' to the end. Strip it.
    int lastValidChar = defaultPath.size() - 1;
    char symbol = defaultPath.at(lastValidChar);
    while(symbol == '/' || symbol == '\\')
    {
        --lastValidChar;
        if(lastValidChar < 0)
        {
            std::string backupPath = "/";
            std::cerr << "failed to find a default dirrectory. Setting it to: " << backupPath << std::endl;
            SDL_free(sdlDefaultPath);
            return backupPath;
        }
        symbol = defaultPath.at(lastValidChar);
    }
    defaultPath.erase(lastValidChar + 1);
    SDL_free(sdlDefaultPath);
    return defaultPath;
}

std::string RomLoader::CombinePath(const std::string& path, const std::string& file)
{
    return path + "/" + file;
}

bool RomLoader::IsDir( const std::string& filePath)
{
    struct stat fileStat;
    stat(filePath.c_str(), &fileStat);
    return S_ISDIR(fileStat.st_mode);
}

RomLoader::RomLoader()
{
    this->currentFilesystemPath = this->GetDefaultPath();
}

std::vector<std::string> RomLoader::ListFilesInPath(const std::string& fileSystemPath ) const
{
    DIR * dp = opendir( fileSystemPath.c_str() );
    if(dp == nullptr)
    {
        std::cerr << "failed to find dirrectory: " << fileSystemPath << std::endl;
        return {};
    }

    std::vector<std::string> fileList;

    dirent * entry = readdir(dp);
    while(entry != nullptr)
    {
        fileList.push_back(entry->d_name);

        entry = readdir(dp);
    }

    closedir( dp );
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