#include "INesParser.h"

std::unique_ptr<INes> INesParser::ParseINes(const std::string& romFilePath)
{
    std::ifstream fileStream( romFilePath, std::ios::in | std::ios::binary );

    const int bufferSize = 8;
    char buffer[bufferSize];

    fileStream.read(buffer, bufferSize);
    std::cout << buffer[0] << std::endl;
    std::unique_ptr<INes> rom(new INes());
    return rom;
}