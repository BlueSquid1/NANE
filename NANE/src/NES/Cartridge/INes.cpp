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


std::unique_ptr<std::vector<byte>> INes::GetPrgRomData()
{
    return std::move(this->prgRomData);
}

void INes::SetPrgRomData(std::unique_ptr<std::vector<byte>> prgRomData)
{
    this->prgRomData = std::move(prgRomData);
}

std::unique_ptr<std::vector<byte>> INes::GetChrRomData()
{
    return std::move(this->chrRomData);
}

void INes::SetChrRomData(std::unique_ptr<std::vector<byte>> chrRomData)
{
    this->chrRomData = std::move(chrRomData);
}