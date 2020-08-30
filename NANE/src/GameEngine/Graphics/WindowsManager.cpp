#include "WindowsManager.h"

#include "CustomEventMgr.h"

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
	const int FILE_BAR_HEIGHT = 22;
}

bool WindowManager::Init(bool showFileMenu)
{
	this->enableMenuBar = showFileMenu;
	
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

    this->gWindow = SDL_CreateWindow("NES-NX", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1, 1, SDL_WINDOW_HIDDEN);
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
	SDL_Color cpuForgroundColour;
	cpuForgroundColour.r = 0xFF;
	cpuForgroundColour.g = 0xFF;
	cpuForgroundColour.b = 0xFF;
	cpuForgroundColour.a = 0xFF;
	SDL_Color cpuBackgroundColour;
	cpuBackgroundColour.r = 0x00;
	cpuBackgroundColour.g = 0x00;
	cpuBackgroundColour.b = 0x00;
	cpuBackgroundColour.a = 0xFF;
	this->cpuWindow = std::make_unique<TextWindow>(this->gRenderer, TextDirection::bottomLeft, cpuForgroundColour, cpuBackgroundColour);
	this->colourDisplayWindow = std::make_unique<TextureWindow>(this->gRenderer, COLOUR_DISPLAY_WIDTH, COLOUR_DISPLAY_HEIGHT);
	this->playerOneInputs = std::make_unique<TextureWindow>(this->gRenderer, PLAYER_ONE_INPUTS_WIDTH, PLAYER_ONE_INPUTS_HEIGHT);
	SDL_Color fpsTextColour;
	fpsTextColour.r = 0xFF;
	fpsTextColour.g = 0xFF;
	fpsTextColour.b = 0xAA;
	fpsTextColour.a = 0xFF;
	SDL_Color transparentColor;
	transparentColor.a = 0x00;
	this->fpsDisplay = std::make_unique<TextWindow>(this->gRenderer, TextDirection::topLeft, fpsTextColour, transparentColor);
	this->openRomWindow = std::make_unique<TextWindow>(this->gRenderer, TextDirection::centred, cpuForgroundColour, cpuBackgroundColour);
	this->menuBar = std::make_unique<MenuBar>(this->gRenderer);

	this->ChangeScaleFactor(2);
	SDL_ShowWindow(this->gWindow);
	SDL_SetWindowPosition(this->gWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	return true;
}

void WindowManager::UpdateLayout()
{
	// Apply scaling to windows
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

	// calculate useful dimensions
	int totalWidth;
	int totalHeight;
	int mainOffsetHeight;
	int mainOffsetWidth;

	if(this->curWindowView == WindowView::Disassemble)
	{
		totalWidth = BORDER_WIDTH + mainWindowWidth + BORDER_WIDTH + chrRomWidth + BORDER_WIDTH;
		totalHeight = BORDER_WIDTH + chrRomHeight + BORDER_WIDTH + cpuDisplayHeight + BORDER_WIDTH;
		mainOffsetHeight = BORDER_WIDTH;
		mainOffsetWidth = BORDER_WIDTH;
	}
	else if (this->curWindowView == WindowView::Simple)
	{
		totalWidth = mainWindowWidth;
		totalHeight = mainWindowHeight;
		mainOffsetHeight = 0;
		mainOffsetWidth = 0;
	}

	if(this->enableMenuBar)
	{
		// make space for the memu bar
		mainOffsetHeight += FILE_BAR_HEIGHT;
		totalHeight += FILE_BAR_HEIGHT;
	}

	// Update window dimensions
	this->mainWindow->SetDimensions(mainOffsetWidth, mainOffsetHeight, mainWindowWidth, mainWindowHeight);
	this->chrRomWindow->SetDimensions(mainWindowWidth + BORDER_WIDTH + mainOffsetWidth, mainOffsetHeight, chrRomWidth, chrRomHeight);
	this->cpuWindow->SetDimensions(mainWindowWidth + BORDER_WIDTH + mainOffsetWidth, chrRomHeight + BORDER_WIDTH + mainOffsetHeight, cpuDisplayWidth, cpuDisplayHeight);
	this->colourDisplayWindow->SetDimensions(mainOffsetWidth, mainWindowHeight + BORDER_WIDTH + mainOffsetHeight, colourDisplayWidth, colourDisplayHeight);
	this->playerOneInputs->SetDimensions(colourDisplayWidth + BORDER_WIDTH + mainOffsetWidth, mainWindowHeight + BORDER_WIDTH + mainOffsetHeight, playerOneInputsWidth, playerOneInputsHeight); 
	this->fpsDisplay->SetDimensions(mainOffsetWidth, mainOffsetHeight, FPS_COUNTER_WIDTH, FPS_COUNTER_HEIGHT);
	this->openRomWindow->SetDimensions(0, mainOffsetHeight, totalWidth, totalHeight - mainOffsetHeight);
	if(this->enableMenuBar)
	{
		this->menuBar->SetDimensions(0, 0, totalWidth, FILE_BAR_HEIGHT);
	}

	//update window size
	SDL_SetWindowSize(this->gWindow, totalWidth, totalHeight);
}

void WindowManager::RenderMainDisplay(Nes& nesEmulator,unsigned int fps)
{
	const Matrix<rawColour>& mainDisplay = nesEmulator.GetFrameDisplay();
	this->mainWindow->Display(mainDisplay);
	if(this->enableFpsCounter)
	{
		fpsStringStream.str("");
		fpsStringStream << fps << "FPS";
		const std::string fpsText = fpsStringStream.str();
		this->fpsDisplay->Display(fpsText);
	}
}

void WindowManager::ChangeScaleFactor(int newScaleFactor)
{
	this->windowScale = newScaleFactor;
	this->UpdateLayout();
}

void WindowManager::ChangeViewType(WindowView newWindowView)
{
	this->curWindowView = newWindowView;
	this->UpdateLayout();
}

void WindowManager::HandleEvent(const SDL_Event& e)
{
	if(e.type == CustomEventMgr::GetInstance()->GetCustomEventType())
	{
		MenuEvents menuEvent = (MenuEvents)e.user.code;
		switch(menuEvent)
		{
			case MenuEvents::SimpleView:
			{
				this->ChangeViewType(WindowView::Simple);
				break;
			}
			case MenuEvents::DisassembleView:
			{
				this->ChangeViewType(WindowView::Disassemble);
				break;
			}
			case MenuEvents::ScaleFactor1:
			{
				this->ChangeScaleFactor(1);
				break;
			}
			case MenuEvents::ScaleFactor2:
			{
				this->ChangeScaleFactor(2);
				break;
			}
			case MenuEvents::ScaleFactor3:
			{
				this->ChangeScaleFactor(3);
				break;
			}
			case MenuEvents::ScaleFactor4:
			{
				this->ChangeScaleFactor(4);
				break;
			}
			case MenuEvents::ScaleFactor5:
			{
				this->ChangeScaleFactor(5);
				break;
			}
			case MenuEvents::ShowFpsCounter:
			{
				this->enableFpsCounter = !this->enableFpsCounter;
				break;
			}
			case MenuEvents::OpenRom:
			{
				this->curWindowView = WindowView::OpenRom;
			}
		}
	}
	this->menuBar->HandleEvent(e);
}

bool WindowManager::Display(Nes& nesEmulator,unsigned int fps, const std::string& fileSystemText)
{
    //clear the screen
	SDL_SetRenderDrawColor(this->gRenderer, 0x80, 0x80, 0x80, 0xFF);
	SDL_RenderClear(this->gRenderer);

	//render windows
	switch(this->curWindowView)
	{
		case WindowView::Disassemble:
		{
			this->RenderMainDisplay(nesEmulator, fps);

			std::unique_ptr<Matrix<rawColour>> chrRomDisplay = nesEmulator.GeneratePatternTables();
			this->chrRomWindow->Display(*chrRomDisplay);

			std::string cpuText = nesEmulator.GenerateCpuScreen();
			this->cpuWindow->Display(cpuText);

			std::unique_ptr<Matrix<rawColour>> colourPalettesDisplay = nesEmulator.GenerateColourPalettes();
			this->colourDisplayWindow->Display(*colourPalettesDisplay);

			std::unique_ptr<Matrix<rawColour>> playerOneDisplay = nesEmulator.GenerateControllerState();
			this->playerOneInputs->Display(*playerOneDisplay);
			break;
		}
		case WindowView::Simple:
		{
			this->RenderMainDisplay(nesEmulator, fps);
			break;
		}
		case WindowView::OpenRom:
		{
			this->openRomWindow->Display(fileSystemText);
			break;
		}
	}

	if(this->enableMenuBar)
	{
		this->menuBar->Display();
	}

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