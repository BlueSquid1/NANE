#include "FileNavigator.h"

#include "CustomEventMgr.h"

FileNavigator::FileNavigator(SDL_Renderer* gRenderer)
: IWindow(gRenderer), 
  filePathWindow(gRenderer, TextDirection::centred), 
  hoveringFileCurser(gRenderer, TextDirection::centred)
{
    Uint32 fileSelectedEvent = (Uint32)CustomEvents::FileSelected;
    this->triggerEvent = CustomEventMgr::GetInstance()->GenerateEvent(fileSelectedEvent);
}

void FileNavigator::SetFileSystemData(const std::string& filePath, const std::vector<std::string>& files)
{
    //file bread crumb
    this->filePath = filePath;
    
    //file curser shared values
    const int fileCurserMargin = 10;
    const int curserWidth = 20;
    int curserHeight = 20;

    //file list
    this->files.clear();
    const int filePadding = 5;
    const SDL_Rect& titleDimensions = filePathWindow.GetDimensions();
    int buttonPoxX = fileCurserMargin + curserWidth + filePadding;
    int buttonPoxY = titleDimensions.y + titleDimensions.h + filePadding;
    for(const std::string& file_name : files)
    {
        Uint32 fileSelectedEvent = (Uint32)CustomEvents::FileSelected;
        PushButton fileBtn(this->gRenderer, file_name, &fileSelectedEvent);
        SDL_Rect buttonSize = fileBtn.CalculateMinSize();
        fileBtn.SetDimensions(buttonPoxX, buttonPoxY, buttonSize.w, buttonSize.h);
        this->files.push_back(fileBtn);

        curserHeight = buttonSize.h;
        buttonPoxY += buttonSize.h + filePadding;
    }

    //file curser
    this->curFileIndex = 0;
    SDL_Rect selectFileDimensions = this->files.at(this->curFileIndex).GetDimensions();
    hoveringFileCurser.SetDimensions(fileCurserMargin, selectFileDimensions.y, curserWidth, curserHeight);
}

void FileNavigator::SetDimensions(int posX, int posY, int width, int height)
{
    IWindow::SetDimensions(posX, posY, width, height);

    const int breadcrumbHeight = 25;
    filePathWindow.SetDimensions(posX, posY, width, breadcrumbHeight);
}

void FileNavigator::HandleEvent(const SDL_Event& e)
{
    //curser
    for(unsigned int i = 0; i < this->files.size(); ++i)
    {
        this->files[i].HandleEvent(e);

        if(this->files[i].GetIsHighlighted())
        {
            this->curFileIndex = i;
        }
    }

    //arrow keys
    if(e.type == SDL_KEYDOWN)
    {
        switch( e.key.keysym.sym )
        {
            case SDLK_UP:
            {
                if(this->curFileIndex > 0)
                {
                    --this->curFileIndex;
                }
                break;
            }
            case SDLK_DOWN:
            {
                if(this->curFileIndex < (this->files.size() - 1))
                {
                    ++this->curFileIndex;
                }
                break;
            }
            case SDLK_RETURN:
            {
                SDL_PushEvent(this->triggerEvent.get());
                break;
            }
        }
    }

    if(this->curFileIndex >= 0 && this->curFileIndex < this->files.size())
    {
        int fileYPos = this->files[this->curFileIndex].GetDimensions().y;
        SDL_Rect oldCurser = hoveringFileCurser.GetDimensions();
        hoveringFileCurser.SetDimensions(oldCurser.x, fileYPos, oldCurser.w, oldCurser.h);
    }
}

void FileNavigator::Display()
{
    for(PushButton& file : this->files)
    {
        file.Display();
    }

    filePathWindow.Display(this->filePath);

    hoveringFileCurser.Display("->");
}

std::string FileNavigator::GetSelectedFile()
{
    return this->files.at(this->curFileIndex).GetDisplayName();
}