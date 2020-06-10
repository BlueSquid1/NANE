#ifndef FONT_MANAGER
#define FONT_MANAGER

#include <memory>
#include <string>
#include <unordered_map>
#include <SDL_ttf.h>

/**
 * SDL_ttf gets confused when working with many fonts. This class ensure all fonts are closed at the end of the program safely.
 */

class FontManager
{
    private:
    static std::unique_ptr<FontManager> instance;

    std::unordered_map<std::string, TTF_Font *> openFonts;
    
    //private constructor
    FontManager();

    public:
    static bool Init();
    static std::unique_ptr<FontManager>& GetInstance();
    static void Close();

    ~FontManager();

    TTF_Font * LoadGenericFont(const std::string& font, int fontPt);
};


#endif