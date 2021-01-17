#pragma once

#include <iostream> //std::cout
#include <memory> //unique_ptr
#include <fstream> //std::ifstream
#include <exception> //exceptions

#include "CartridgeMapping/ICartridge.h"
#include "CartridgeMapping/CartridgeMapping0.h"
#include "NES/Memory/BitUtil.h"
#include "INes.h"

/**
 * responsible for parsing a INET ROM file.
 */
class CartridgeLoader
{
    private:
    //parses and updates ROM with the head section
    bool UpdateHead(std::shared_ptr<INes> rom, std::ifstream * fileStream);
    //parses and updates ROM with the trainer section
    bool UpdateTrainer(std::shared_ptr<INes> rom, std::ifstream * fileStream);
    //parses and updates ROM with the Prg section
    bool UpdatePrgRomData(std::shared_ptr<INes> rom, std::ifstream * fileStream);
    //parses and updates ROM with the Chr section
    bool UpdateChrRomData(std::shared_ptr<INes> rom, std::ifstream * fileStream);

    public:
    /**
     * parsers a ROM file.
     * @param romFilePath the file path to the .nes file to be parsed into a INES format. e.g. "/var/example.nes".
     * @return the parsed ROM file.
     */
    std::shared_ptr<INes> ParseINes(const std::string & romFilePath);


    std::shared_ptr<ICartridge> LoadCartridge(const std::string & romFilePath);
};