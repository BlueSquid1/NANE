#include "INesParser.h"

INesParser::INesParser()
{
    this->rom = std::unique_ptr<INes>(new INes());
}

bool INesParser::UpdateHead( std::ifstream * fileStream )
{
    const int headerSize = 16;
    std::vector<byte> header(headerSize);
    fileStream->read( (char *) &header[0], headerSize);

    std::vector<byte> headerString = {0x4E, 0x45, 0x53, 0x1A};
    if( BitUtil::VecEqual( BitUtil::VecSplit(&header, 0, 4).get(), &headerString) == false )
    {
        std::cerr << "not a INES file" << std::endl;
        return false;
    }

    this->rom->SetPrgRomLen(header[4] * 16384);
    this->rom->SetChrRomLen(header[5] * 8192);
    byte flag_6 = header[6];
    bool mirroringVertical = BitUtil::GetBits(flag_6, 0);
    bool containsBattery = BitUtil::GetBits(flag_6, 1);
    bool trainerPresent = BitUtil::GetBits(flag_6, 2);
    bool ignoreMirror = BitUtil::GetBits(flag_6, 3);
    byte lowerNybbleMapperNum = BitUtil::GetBits(flag_6, 4, 7);

    INes::MirrorType mirrorType;
    if(ignoreMirror == true)
    {
        if(mirroringVertical == false)
        {
            mirrorType = INes::disabled;
        }
        else
        {
            mirrorType = INes::fourScreen;
        }
    }
    else
    {
        if(mirroringVertical == true)
        {
            mirrorType = INes::vertical;
        }
        else
        {
            mirrorType = INes::horizontal;
        }
    }
    this->rom->SetMirroringType(mirrorType);
    this->rom->SetBatteryPresent(containsBattery);
    this->rom->SetTrainerPresent(trainerPresent);

    byte flag_7 = header[7];
    bool isVsUniSystem = BitUtil::GetBits(flag_7, 0);
    bool isPlayChoice = BitUtil::GetBits(flag_7, 1);
    byte Nes2Flag = BitUtil::GetBits(flag_7, 2,3);
    if( Nes2Flag == 2 )
    {
        throw std::invalid_argument("Rom is NES2 format which hasn't been implemented yet in this emulator.");
    }
    byte upperNybbleMapperNum = BitUtil::GetBits(flag_7, 4, 7);
    upperNybbleMapperNum = upperNybbleMapperNum << 4;
    byte mapperNum = upperNybbleMapperNum | lowerNybbleMapperNum;
    this->rom->SetMapperNumber(mapperNum);

    return true;
}

bool INesParser::UpdateTrainer(std::ifstream * fileStream)
{
    if(this->rom->GetTrainerPresent() == true)
    {
        const int trainerSize = 512;
        std::vector<byte> trainer(trainerSize);
        fileStream->read( (char *) &trainer[0], trainerSize);
        std::cerr << "trainer presented in ROM. This hasn't been implemented yet" << std::endl;
    }
    return true;
}

bool INesParser::UpdatePrgRomData(std::ifstream * fileStream)
{
    unsigned int prgRomSize = this->rom->GetPrgRomLen();
    std::shared_ptr<std::vector<byte>> prgRom(new std::vector<byte>(prgRomSize));
    fileStream->read( (char *) &(*prgRom)[0], prgRomSize);
    this->rom->SetPrgRomData(prgRom);
    return true;
}

bool INesParser::UpdateChrRomData(std::ifstream * fileStream)
{
    unsigned int chrRomSize = this->rom->GetChrRomLen();
    std::shared_ptr<std::vector<byte>> chrRom(new std::vector<byte>(chrRomSize));
    fileStream->read( (char *) &(*chrRom)[0], chrRomSize);
    this->rom->SetChrRomData(chrRom);
    return true;
}



std::unique_ptr<INes> INesParser::ParseINes(const std::string& romFilePath)
{
    std::ifstream fileStream( romFilePath, std::ios::in | std::ios::binary );
    if( fileStream.is_open() == false )
    {
        std::cerr << "can't open file at path: " << romFilePath << std::endl;
        return NULL;
    }

    //read header
    bool headRet = this->UpdateHead( &fileStream );
    if(headRet == false)
    {
        return NULL;
    }

    //read trainer if present
    bool trainerRet = this->UpdateTrainer( &fileStream );
    if(trainerRet == false)
    {
        return NULL;
    }

    //read Program Rom
    bool prgRomRet = this->UpdatePrgRomData( &fileStream );
    if(prgRomRet == false)
    {
        return NULL;
    }

    //read chr Rom
    bool chrRomRet = this->UpdateChrRomData( &fileStream );
    if(chrRomRet == false)
    {
        return NULL;
    }

    return std::move(this->rom);
}