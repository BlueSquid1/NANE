#include "INes.h"

unsigned int INes::GetPrgRomLen()
{
    return this->prgRomLen;
}

void INes::SetPrgRomLen(unsigned int prgRomLen)
{
    this->prgRomLen = prgRomLen;
}

unsigned int INes::GetChrRomLen()
{
    return this->chrRomLen;
}

void INes::SetChrRomLen(unsigned int chrRomLen)
{
    this->chrRomLen = chrRomLen;
}

unsigned int INes::GetMapperNumber()
{
    return this->mapperNumber;
}

void INes::SetMapperNumber(unsigned int mapperNumber)
{
    this->mapperNumber = mapperNumber;
}

INes::MirrorType INes::GetMirroringType()
{
    return this->mirroringType;
}

void INes::SetMirroringType(MirrorType mirroringType)
{
    this->mirroringType = mirroringType;
}

bool INes::GetBatteryPresent()
{
    return this->isBatteryPresent;
}

void INes::SetBatteryPresent(bool isBatteryPresent)
{
    this->isBatteryPresent = isBatteryPresent;
}

bool INes::GetTrainerPresent()
{
    return this->isTrainerPresent;
}

void INes::SetTrainerPresent(bool isTrainerPresent)
{
    this->isTrainerPresent = isTrainerPresent;
}


std::shared_ptr<std::vector<byte>> INes::GetPrgRomData()
{
    return this->prgRomData;
}

void INes::SetPrgRomData(std::shared_ptr<std::vector<byte>> prgRomData)
{
    this->prgRomData = prgRomData;
}

std::shared_ptr<std::vector<byte>> INes::GetChrRomData()
{
    return this->chrRomData;
}

void INes::SetChrRomData(std::shared_ptr<std::vector<byte>> chrRomData)
{
    this->chrRomData = chrRomData;
}