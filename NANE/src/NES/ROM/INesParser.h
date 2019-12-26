#ifndef INES_PARSER
#define INES_PARSER

#include <iostream>
#include <memory> //unique_ptr
#include <fstream> //std::ifstream

#include "INes.h"

class INesParser
{
    private:
    INesParser();

    public:
    static std::unique_ptr<INes> ParseINes(const std::string& romFilePath);
};

#endif