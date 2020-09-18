#ifndef ROM_LOADER
#define ROM_LOADER

#include <string>
#include <memory>
#include <vector>

class RomLoader
{
    private:
    std::string currentFilesystemPath = "A";

    public:
    std::vector<std::string> ListFilesInPath(const std::string& filesystemPath );

    //getter/setters
    std::string GetCurrentFilesystemPath() const;
    void SetCurrentFilesystemPath(const std::string& path);
};

#endif