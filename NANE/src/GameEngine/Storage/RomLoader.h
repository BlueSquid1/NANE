#ifndef ROM_LOADER
#define ROM_LOADER

#include <string>
#include <memory>
#include <vector>

class RomLoader
{
    private:
    std::string currentFilesystemPath;

    std::string GetDefaultPath();

    public:
    static std::string CombinePath(const std::string& path, const std::string& file);
    static bool IsDir( const std::string& filePath);

    RomLoader();

    std::vector<std::string> ListFilesInPath(const std::string& filesystemPath ) const;

    //getter/setters
    std::string GetCurrentFilesystemPath() const;
    void SetCurrentFilesystemPath(const std::string& path);
};

#endif