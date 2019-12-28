#ifndef INES_PARSER
#define INES_PARSER

#include <iostream> //std::cout
#include <memory> //unique_ptr
#include <fstream> //std::ifstream
#include <exception> //exceptions

#include "../Util/BitUtil.h"
#include "INes.h"

/**
 * responsible for parsing a INET ROM file.
 */
class INesParser
{
    private:
    //parse ROM file
    std::unique_ptr<INes> rom = NULL;

    //parses and updates ROM with the head section
    bool UpdateHead(std::ifstream * fileStream);
    //parses and updates ROM with the trainer section
    bool UpdateTrainer(std::ifstream * fileStream);
    //parses and updates ROM with the Prg section
    bool UpdatePrgRomData(std::ifstream * fileStream);
    //parses and updates ROM with the Chr section
    bool UpdateChrRomData(std::ifstream * fileStream);

    public:
    /**
     * constructor
     */
    INesParser();
    /**
     * parsers a ROM file.
     * @param romFilePath the file path to the .nes file to be parsed into a INES format. e.g. "/var/example.nes".
     * @return the parsed ROM file.
     */
    std::unique_ptr<INes> ParseINes(const std::string& romFilePath);
};

#endif