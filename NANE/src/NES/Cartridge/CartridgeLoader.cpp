#include "CartridgeLoader.h"

bool CartridgeLoader::UpdateHead( std::unique_ptr<INes> & rom, std::ifstream * fileStream )
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

    rom->SetPrgRomLen(header[4] * 16384);
    rom->SetChrRomLen(header[5] * 8192);
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
            mirrorType = INes::one_way;
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
    rom->SetMirroringType(mirrorType);
    rom->SetBatteryPresent(containsBattery);
    rom->SetTrainerPresent(trainerPresent);

    byte flag_7 = header[7];
    byte Nes2Flag = BitUtil::GetBits(flag_7, 2,3);
    if( Nes2Flag == 2 )
    {
        throw std::invalid_argument("Rom is NES2 format which hasn't been implemented yet in this emulator.");
    }
    byte upperNybbleMapperNum = BitUtil::GetBits(flag_7, 4, 7);
    upperNybbleMapperNum = upperNybbleMapperNum << 4;
    byte mapperNum = upperNybbleMapperNum | lowerNybbleMapperNum;
    rom->SetMapperNumber(mapperNum);

    return true;
}

bool CartridgeLoader::UpdateTrainer( std::unique_ptr<INes> & rom, std::ifstream * fileStream)
{
    if(rom->GetTrainerPresent() == true)
    {
        const int trainerSize = 512;
        std::vector<byte> trainer(trainerSize);
        fileStream->read( (char *) &trainer[0], trainerSize);
        std::cerr << "trainer presented in ROM. This hasn't been implemented yet" << std::endl;
    }
    return true;
}

bool CartridgeLoader::UpdatePrgRomData( std::unique_ptr<INes> & rom, std::ifstream * fileStream)
{
    unsigned int prgRomSize = rom->GetPrgRomLen();
    std::shared_ptr<std::vector<byte>> prgRom(new std::vector<byte>(prgRomSize));
    fileStream->read( (char *) prgRom->data(), prgRomSize);
    rom->SetPrgRomData(prgRom);
    return true;
}

bool CartridgeLoader::UpdateChrRomData( std::unique_ptr<INes> & rom, std::ifstream * fileStream)
{
    unsigned int chrRomSize = rom->GetChrRomLen();
    std::shared_ptr<std::vector<byte>> chrRom(new std::vector<byte>(chrRomSize));
    fileStream->read( (char *) chrRom->data(), chrRomSize);
    rom->SetChrRomData(chrRom);
    return true;
}



std::unique_ptr<INes> CartridgeLoader::ParseINes(const std::string& romFilePath)
{
    std::ifstream fileStream( romFilePath, std::ios::in | std::ios::binary );
    if( fileStream.is_open() == false )
    {
        std::cerr << "can't open file at path: " << romFilePath << std::endl;
        return NULL;
    }

    //parse ROM file
    std::unique_ptr<INes> rom = std::unique_ptr<INes>(new INes());

    //read header
    bool headRet = this->UpdateHead( rom, &fileStream );
    if(headRet == false)
    {
        return NULL;
    }

    //read trainer if present
    bool trainerRet = this->UpdateTrainer( rom, &fileStream );
    if(trainerRet == false)
    {
        return NULL;
    }

    //read Program Rom
    bool prgRomRet = this->UpdatePrgRomData( rom, &fileStream );
    if(prgRomRet == false)
    {
        return NULL;
    }

    //read chr Rom
    bool chrRomRet = this->UpdateChrRomData( rom, &fileStream );
    if(chrRomRet == false)
    {
        return NULL;
    }

    return rom;
}

std::unique_ptr<ICartridge> CartridgeLoader::LoadCartridge(const std::string & romFilePath)
{
    std::unique_ptr<INes> iNesRom = this->ParseINes(romFilePath);
    if(iNesRom == NULL)
    {
        return NULL;
    }

    std::unique_ptr<ICartridge> returnCartridge;
    switch(iNesRom->GetMapperNumber())
    {
        case 0:
            returnCartridge = std::unique_ptr<ICartridge>( new CartridgeMapping0() );
            break;
        default:
            std::cerr << "cartrdige mapping number: " << iNesRom->GetMapperNumber() << " hasn't been implemented." << std::endl;
            return NULL;
            break;
    }

    bool loadRet = returnCartridge->LoadINes(std::move(iNesRom));
    if(loadRet == false)
    {
        std::cerr << "failed to load INES into mapping: " << returnCartridge->GetMapNumber() << std::endl;
        return NULL;
    }

    return returnCartridge;
}