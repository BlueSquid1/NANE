#ifndef CARTRIDGE_LOADER
#define CARTRIDGE_LOADER

#include <iostream> //std::cout
#include <memory> //unique_ptr
#include <fstream> //std::ifstream
#include <exception> //exceptions

#include "CartridgeMapping/ICartridge.h"
#include "CartridgeMapping/CartridgeMapping0.h"
#include "NES/Util/BitUtil.h"
#include "INes.h"

/**
 * responsible for parsing a INET ROM file.
 */
class CartridgeLoader
{
    private:
    //parses and updates ROM with the head section
    bool UpdateHead(std::unique_ptr<INes> & rom, std::ifstream * fileStream);
    //parses and updates ROM with the trainer section
    bool UpdateTrainer(std::unique_ptr<INes> & rom, std::ifstream * fileStream);
    //parses and updates ROM with the Prg section
    bool UpdatePrgRomData(std::unique_ptr<INes> & rom, std::ifstream * fileStream);
    //parses and updates ROM with the Chr section
    bool UpdateChrRomData(std::unique_ptr<INes> & rom, std::ifstream * fileStream);

    public:
    /**
     * parsers a ROM file.
     * @param romFilePath the file path to the .nes file to be parsed into a INES format. e.g. "/var/example.nes".
     * @return the parsed ROM file.
     */
    std::unique_ptr<INes> ParseINes(const std::string & romFilePath);


    std::unique_ptr<ICartridge> LoadCartridge(const std::string & romFilePath);
};

#endif