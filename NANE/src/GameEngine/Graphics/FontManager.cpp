#include "FontManager.h"

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

TTF_Font * FontManager::LoadGenericFont(const std::string& font, int fontPt)
{
    //check if font is already loaded
    std::string key = font + std::to_string(fontPt);
    if(this->openFonts.count(key) >= 1)
    {
        return this->openFonts.at(key);
    }
    TTF_Font * ttf_font = TTF_OpenFont( font.c_str(), fontPt );
    this->openFonts[key] = ttf_font;
    return ttf_font;
}