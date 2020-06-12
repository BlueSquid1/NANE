#include "FontManager.h"

#include <exception>

std::unique_ptr<FontManager> FontManager::instance = NULL;

FontManager::FontManager()
{

}

FontManager::~FontManager()
{
    for( const auto& font : this->openFonts)
    {
        TTF_CloseFont(font.second);
    }
}

bool FontManager::Init()
{
    if(TTF_Init() < 0)
	{
		return false;
	}
    return true;
}

std::unique_ptr<FontManager>& FontManager::GetInstance()
{
    if(instance == NULL)
    {
        instance = std::unique_ptr<FontManager>(new FontManager());
    }
    return instance;
}

void FontManager::Close()
{
    instance.reset();
}

TTF_Font * FontManager::LoadFont(FontType type)
{
    int fontPt;
    std::string fontName;
    switch(type)
    {
        case FontType::defaultFont:
        {
            fontPt = 15;
            fontName = "VeraMono.ttf";
            break;
        }
        default:
        {
            throw std::invalid_argument("invalid font type");
        }
    }

    //check if font is already loaded
    if(this->openFonts.count(type) >= 1)
    {
        return this->openFonts.at(type);
    }
    TTF_Font * ttf_font = TTF_OpenFont( fontName.c_str(), fontPt );
    this->openFonts[type] = ttf_font;
    return ttf_font;
}