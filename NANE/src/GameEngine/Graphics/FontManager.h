#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <SDL_ttf.h>

/**
 * SDL_ttf gets confused when working with many fonts. This class ensure all fonts are closed at the end of the program safely.
 */

enum class FontType
{
    defaultFont
};

class FontManager
{
    private:
    static std::unique_ptr<FontManager> instance;

    std::unordered_map<FontType, TTF_Font *> openFonts;

    TTF_Font * LoadGenericFont(const std::string& font, int fontPt);
    
    //private constructor
    FontManager();

    public:
    static bool Init();
    static std::unique_ptr<FontManager>& GetInstance();
    static void Close();

    ~FontManager();

    TTF_Font * LoadFont(FontType type = FontType::defaultFont);
};