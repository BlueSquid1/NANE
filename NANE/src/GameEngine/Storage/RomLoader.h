#ifndef ROM_LOADER
#define ROM_LOADER

#include <string>
#include <memory>

class RomLoader
{
    private:
    std::string currentFilesystemPath;

    public:
    std::string ListFilesInPath(const std::string& filesystemPath );
    std::string GetFilesAtCurrentPath();
};

#endif