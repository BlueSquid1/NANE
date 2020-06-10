#include "WindowsManager.h"

namespace
{
	// scalable dimensions
	const int MAIN_WINDOW_WIDTH = 256;
	const int MAIN_WINDOW_HEIGHT = 240;
	const int CHR_ROM_WIDTH = 256;
	const int CHR_ROM_HEIGHT = 128;
	const int CPU_DISPLAY_WIDTH = 256;
	const int CPU_DISPLAY_HEIGHT = 200;
	const int COLOUR_DISPLAY_WIDTH = 42;
	const int COLOUR_DISPLAY_HEIGHT = 16;
	const int PLAYER_ONE_INPUTS_WIDTH = 50;
	const int PLAYER_ONE_INPUTS_HEIGHT = 8;

	// fixed dimensions
	const int BORDER_WIDTH = 5;
	const int FPS_COUNTER_WIDTH = 100;
	const int FPS_COUNTER_HEIGHT = 100;
	const int FILE_BAR_HEIGHT = 25;
}

bool WindowManager::Init()
{
	std::cout << "initalizing SDL" << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		std::cerr << "can't start SDL. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	if(FontManager::Init() == false)
	{
		std::cerr << "can't start font manager. TTF error: " << TTF_GetError() << std::endl;
		return false;
	}

    this->gWindow = SDL_CreateWindow("NES-NX", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1, 1, SDL_WINDOW_HIDDEN);
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

	//create windows
	this->mainWindow = std::make_unique<TextureWindow>(this->gRenderer, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
	this->chrRomWindow = std::make_unique<TextureWindow>(this->gRenderer, CHR_ROM_WIDTH, CHR_ROM_HEIGHT);
	std::string fontFile = "VeraMono.ttf";
	int fontPt = 15;
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
	this->cpuWindow = std::make_unique<TextWindow>(this->gRenderer, fontFile, fontPt, foregroundColor, backgroundColor, TextDirection::bottomAligned);
	this->colourDisplayWindow = std::make_unique<TextureWindow>(this->gRenderer, COLOUR_DISPLAY_WIDTH, COLOUR_DISPLAY_HEIGHT);
	this->playerOneInputs = std::make_unique<TextureWindow>(this->gRenderer, PLAYER_ONE_INPUTS_WIDTH, PLAYER_ONE_INPUTS_HEIGHT);
	SDL_Color fpsTextColour;
	fpsTextColour.r = 0xFF;
	fpsTextColour.g = 0xFF;
	fpsTextColour.b = 0xAA;
	fpsTextColour.a = 0xFF;
	SDL_Color transparentColor;
	transparentColor.a = 0x00;
	this->fpsDisplay = std::make_unique<TextWindow>(this->gRenderer, fontFile, fontPt, fpsTextColour, transparentColor);
	this->menuBar = std::make_unique<MenuBar>(this->gRenderer);

	this->ChangeScaleFactor(2);
	SDL_ShowWindow(this->gWindow);
	return true;
}

void WindowManager::ChangeScaleFactor(int newScaleFactor)
{
	this->windowScale = newScaleFactor;

	int mainWindowWidth = MAIN_WINDOW_WIDTH * this->windowScale;
	int mainWindowHeight = MAIN_WINDOW_HEIGHT * this->windowScale;
	int chrRomWidth = CHR_ROM_WIDTH * this->windowScale;
	int chrRomHeight = CHR_ROM_HEIGHT * this->windowScale;
	int cpuDisplayWidth = CPU_DISPLAY_WIDTH * this->windowScale;
	int cpuDisplayHeight = CPU_DISPLAY_HEIGHT * this->windowScale;
	int colourDisplayWidth = COLOUR_DISPLAY_WIDTH * this->windowScale;
	int colourDisplayHeight = COLOUR_DISPLAY_HEIGHT * this->windowScale;
	int playerOneInputsWidth = PLAYER_ONE_INPUTS_WIDTH * this->windowScale;
	int playerOneInputsHeight = PLAYER_ONE_INPUTS_HEIGHT * this->windowScale;

	int totalWidth = BORDER_WIDTH + mainWindowWidth + BORDER_WIDTH + chrRomWidth + BORDER_WIDTH;
	int totalHeight = FILE_BAR_HEIGHT + BORDER_WIDTH + chrRomHeight + BORDER_WIDTH + cpuDisplayHeight + BORDER_WIDTH;

	int mainOffsetHeight = FILE_BAR_HEIGHT + BORDER_WIDTH;
	int mainOffsetWidth = BORDER_WIDTH;
	this->mainWindow->SetDimensions(mainOffsetWidth, mainOffsetHeight, mainWindowWidth, mainWindowHeight);
	this->chrRomWindow->SetDimensions(mainWindowWidth + BORDER_WIDTH + mainOffsetWidth, mainOffsetHeight, chrRomWidth, chrRomHeight);
	this->cpuWindow->SetDimensions(mainWindowWidth + BORDER_WIDTH + mainOffsetWidth, chrRomHeight + BORDER_WIDTH + mainOffsetHeight, cpuDisplayWidth, cpuDisplayHeight);
	this->colourDisplayWindow->SetDimensions(mainOffsetWidth, mainWindowHeight + BORDER_WIDTH + mainOffsetHeight, colourDisplayWidth, colourDisplayHeight);
	this->playerOneInputs->SetDimensions(colourDisplayWidth + BORDER_WIDTH + mainOffsetWidth, mainWindowHeight + BORDER_WIDTH + mainOffsetHeight, playerOneInputsWidth, playerOneInputsHeight);
	this->fpsDisplay->SetDimensions(mainOffsetWidth, mainOffsetHeight, FPS_COUNTER_WIDTH, FPS_COUNTER_HEIGHT);
	this->menuBar->SetDimensions(0, 0, totalWidth, FILE_BAR_HEIGHT);

	//update window size
	SDL_SetWindowSize(this->gWindow, totalWidth, totalHeight);
}

bool WindowManager::Display(Nes& nesEmulator,unsigned int fps)
{
    //clear the screen
	SDL_SetRenderDrawColor(this->gRenderer, 0x80, 0x80, 0x80, 0xFF);
	SDL_RenderClear(this->gRenderer);

	//render windows
	const Matrix<rawColour>& mainDisplay = nesEmulator.GetFrameDisplay();
	this->mainWindow->Display(mainDisplay);
	std::unique_ptr<Matrix<rawColour>> chrRomDisplay = nesEmulator.GeneratePatternTables();

	this->chrRomWindow->Display(*chrRomDisplay);

	std::string cpuText = nesEmulator.GenerateCpuScreen();
	this->cpuWindow->Display(cpuText);

	std::unique_ptr<Matrix<rawColour>> colourPalettesDisplay = nesEmulator.GenerateColourPalettes();
	this->colourDisplayWindow->Display(*colourPalettesDisplay);

	std::unique_ptr<Matrix<rawColour>> playerOneDisplay = nesEmulator.GenerateControllerState();
	this->playerOneInputs->Display(*playerOneDisplay);

	//fps
	fpsStringStream.str("");
	fpsStringStream << fps << "FPS";
	const std::string fpsText = fpsStringStream.str();
	this->fpsDisplay->Display(fpsText);

	this->menuBar->Display();

	//Update the surface
	SDL_RenderPresent( this->gRenderer );
    return true;
}

void WindowManager::Close()
{
	std::cout << "Destroying SDL" << std::endl;
	FontManager::Close();
    SDL_DestroyWindow(this->gWindow);
	this->gWindow = NULL;
	SDL_DestroyRenderer(this->gRenderer);
	this->gRenderer = NULL;
    SDL_Quit();
}