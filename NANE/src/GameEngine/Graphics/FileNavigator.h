#pragma once

#include <vector>
#include <string>

#include "IWindow.h"
#include "PushButton.h"
#include "TextWindow.h"

class FileNavigator : public IWindow
{
    private:
    std::unique_ptr<SDL_Event> triggerEvent;
    std::vector<PushButton> files;
    TextWindow filePathWindow;
    TextWindow hoveringFileCurser;

    std::string filePath;
    unsigned int curFileIndex;

    public:
    FileNavigator(SDL_Renderer* gRenderer);

    void SetFileSystemData(const std::string& filePath, const std::vector<std::string>& files);

    void SetDimensions(int posX, int posY, int width, int height) override;

    void HandleEvent(const SDL_Event& e) override;
    
    void Display();

    std::string GetSelectedFile();
};