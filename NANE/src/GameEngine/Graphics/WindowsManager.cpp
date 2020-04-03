#include "WindowsManager.h"

bool WindowManager::Init()
{
	std::cout << "initalizing SDL" << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		std::cerr << "can't start SDL. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	if(TTF_Init() < 0)
	{
		std::cerr << "can't start SDL_TTF. TTF error: " << TTF_GetError() << std::endl;
		return false;
	}

	int mainWindowWidth = 256 * this->windowScale;
	int mainWindowHeight = 240 * this->windowScale;
	int chrRomWidth = 256 * this->windowScale;
	int chrRomHeight = 128 * this->windowScale;
	int cpuDisplayWidth = 256 * this->windowScale;
	int cpuDisplayHeight = 200 * this->windowScale;
	int colourDisplayWidth = 42 * this->windowScale;
	int colourDisplayHeight = 16 * this->windowScale;

	int totalWidth = this->BORDER_WIDTH + mainWindowWidth + this->BORDER_WIDTH + chrRomWidth + BORDER_WIDTH;
	int totalHeight = this->BORDER_WIDTH + chrRomHeight + this->BORDER_WIDTH + cpuDisplayHeight + BORDER_WIDTH;

    this->gWindow = SDL_CreateWindow("NES-NX", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, totalWidth, totalHeight, SDL_WINDOW_SHOWN);
	if (!this->gWindow)
	{
		std::cerr << "can't create window. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	this->gRenderer = SDL_CreateRenderer(this->gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(!this->gRenderer)
	{
		std::cerr << "Can't create renderer. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	this->gFont = TTF_OpenFont( "VeraMono.ttf", 15 );
	if(!this->gFont)
	{
		std::cerr << "failed to open 'VeraMono.tff'. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	//create windows
	bool mainWindowResult = this->mainWindow.Init(this->gRenderer, mainWindowWidth, mainWindowHeight, BORDER_WIDTH, BORDER_WIDTH);
	if(mainWindowResult == false)
	{
		std::cerr << "can't create main window. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}
	bool chrRomWindowResult = this->chrRomWindow.Init(this->gRenderer, chrRomWidth, chrRomHeight, mainWindowWidth + 2 * BORDER_WIDTH, BORDER_WIDTH);
	if(chrRomWindowResult == false)
	{
		std::cerr << "can't create chr rom window. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}
	bool cpuWindowResult = this->cpuWindow.Init(this->gRenderer, cpuDisplayWidth, cpuDisplayHeight, mainWindowWidth + 2 * BORDER_WIDTH, chrRomHeight + 2 * BORDER_WIDTH);
	if(cpuWindowResult == false)
	{
		std::cerr << "can't create cpu window. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}
	bool colourDisplayWindow = this->colourDisplayWindow.Init(this->gRenderer, colourDisplayWidth, colourDisplayHeight, BORDER_WIDTH, mainWindowHeight + 2 * BORDER_WIDTH);
	if(chrRomWindowResult == false)
	{
		std::cerr << "can't create window. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}

void WindowManager::ChangeScaleFactor(int newScaleFactor)
{
	int mainWindowWidth = 256 * this->windowScale;
	int mainWindowHeight = 240 * this->windowScale;
	int chrRomWidth = 256 * this->windowScale;
	int chrRomHeight = 128 * this->windowScale;
	int cpuDisplayWidth = 256 * this->windowScale;
	int cpuDisplayHeight = 200 * this->windowScale;
	int colourDisplayWidth = 42 * this->windowScale;
	int colourDisplayHeight = 16 * this->windowScale;

	int totalWidth = BORDER_WIDTH + mainWindowWidth + BORDER_WIDTH + chrRomWidth + BORDER_WIDTH;
	int totalHeight = BORDER_WIDTH + chrRomHeight + BORDER_WIDTH + cpuDisplayHeight + BORDER_WIDTH;

	this->windowScale = newScaleFactor;
	this->mainWindow.ChangeDimensions(mainWindowWidth, mainWindowHeight, BORDER_WIDTH, BORDER_WIDTH);
	this->chrRomWindow.ChangeDimensions(chrRomWidth, chrRomHeight, mainWindowWidth + BORDER_WIDTH, BORDER_WIDTH);
	this->cpuWindow.ChangeDimensions(cpuDisplayWidth, cpuDisplayHeight, mainWindowWidth + BORDER_WIDTH, chrRomHeight + 2 * BORDER_WIDTH);
	this->colourDisplayWindow.ChangeDimensions(colourDisplayWidth, colourDisplayHeight, BORDER_WIDTH, mainWindowHeight + BORDER_WIDTH);

	//update window size
	SDL_SetWindowSize(this->gWindow, totalWidth, totalHeight);
}

bool WindowManager::Display(Nes& nesEmulator, bool showDisassembly)
{
    //clear the screen
	SDL_SetRenderDrawColor(this->gRenderer, 0x80, 0x80, 0x80, 0xFF);
	SDL_RenderClear(this->gRenderer);

	//render windows
	const Matrix<rawColour>& mainDisplay = nesEmulator.GetFrameDisplay();
	this->mainWindow.Display(mainDisplay);

	if(showDisassembly == true)
	{
		std::unique_ptr<Matrix<rawColour>> chrRomDisplay = nesEmulator.GeneratePatternTables();
		this->chrRomWindow.Display(*chrRomDisplay);

		std::string cpuText = nesEmulator.GenerateCpuScreen();
		SDL_Color foregroundColor;
		foregroundColor.r = 0xFF;
		foregroundColor.g = 0xFF;
		foregroundColor.b = 0xFF;
		foregroundColor.a = 0xFF;
		SDL_Color backgroundColor;
		backgroundColor.r = 0x00;
		backgroundColor.g = 0x00;
		backgroundColor.b = 0x00;
		backgroundColor.a = 0xFF;
		this->cpuWindow.Display(cpuText, this->gFont, foregroundColor, backgroundColor);

		std::unique_ptr<Matrix<rawColour>> colourPalettesDisplay = nesEmulator.GenerateColourPalettes();
		this->colourDisplayWindow.Display(*colourPalettesDisplay);
	}

	//Update the surface
	SDL_RenderPresent( this->gRenderer );
    return true;
}

void WindowManager::Close()
{
	std::cout << "Destroying SDL" << std::endl;
    SDL_DestroyWindow(this->gWindow);
	this->gWindow = NULL;
	SDL_DestroyRenderer(this->gRenderer);
	this->gRenderer = NULL;
	TTF_CloseFont(this->gFont);
	this->gFont = NULL;
    SDL_Quit();
}