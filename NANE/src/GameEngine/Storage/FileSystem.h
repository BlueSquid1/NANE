#ifndef FILE_SYSTEM
#define FILE_SYSTEM

#include <string>
#include <memory>
#include <vector>

class FileSystem
{
    private:
    std::string currentFilesystemPath;

    std::string GetDefaultPath();

    public:
    static std::string CombinePath(const std::string& path, const std::string& file);
    static bool IsDir( const std::string& filePath);

    FileSystem();

    std::vector<std::string> ListFilesInPath(const std::string& filesystemPath ) const;

    /**
     * @brief Writes the contents to a new file.
     * @param fileName the file to create from the current filesystem path.
     * @param data file data to write.
     */
    bool WriteToFile(const std::string& fileName, const std::string& data) const;

    //getter/setters
    std::string GetCurrentFilesystemPath() const;
    void SetCurrentFilesystemPath(const std::string& path);
};

#endif