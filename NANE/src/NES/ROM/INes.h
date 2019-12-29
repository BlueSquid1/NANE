#ifndef INES
#define INES

#include <iostream>

#include "NES/Util/BitUtil.h"

/**
 * data structure for a INES rom (i.e. a .nes file)
 */
class INes
{
    public:
    /**
     * Mirror type specified in the NES ROM.
     */
    enum MirrorType
    {
        disabled, /**< No mirror */
        horizontal, /**< mirror the screen on the horizontal axis */
        vertical, /**< mirror the screen on the vertical axis */
        fourScreen /**< split the screen into four sections */
    };

    private:
    //in bytes
    unsigned int prgRomLen = 0;
    //in bytes
    unsigned int chrRomLen = 0;
    //the NES ROM mapping number
    unsigned int mapperNumber = 0;
    //NES ROM mirroring state
    MirrorType mirroringType = horizontal;

    //battery present means that values stored in PRG RAM ($6000-7FFF) are non-volitile
    bool isBatteryPresent = false;

    //if a trainer is present
    bool isTrainerPresent = false;

    //stores Prg ROM data
    std::shared_ptr<std::vector<byte>> prgRomData = NULL;

    //stores Chr ROM data
    std::shared_ptr<std::vector<byte>> chrRomData = NULL;

    //getters and setters
    public:
    unsigned int GetPrgRomLen();
    void SetPrgRomLen(unsigned int prgRomLen);
    
    unsigned int GetChrRomLen();
    void SetChrRomLen(unsigned int chrRomLen);

    unsigned int GetMapperNumber();
    void SetMapperNumber(unsigned int mapperNumber);

    MirrorType GetMirroringType();
    void SetMirroringType(MirrorType mirroringType);

    bool GetBatteryPresent();
    void SetBatteryPresent(bool isBatteryPresent);

    bool GetTrainerPresent();
    void SetTrainerPresent(bool isTrainerPresent);

    std::shared_ptr<std::vector<byte>> GetPrgRomData();
    void SetPrgRomData(std::shared_ptr<std::vector<byte>> prgRomData);

    std::shared_ptr<std::vector<byte>> GetChrRomData();
    void SetChrRomData(std::shared_ptr<std::vector<byte>> chrRomData);
};

#endif