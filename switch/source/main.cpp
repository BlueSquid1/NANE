#include <iostream> //cout
#include <stdlib.h>
#include <stdio.h>

#include "network/Logger.h"

#include <SDL.h>
#include <switch.h>

int main(int argc, char *argv[])
{
    Logger::Initalize("10.48.119.4", STDOUT_FILENO, STDERR_FILENO);
    std::cout << "launching program" << std::endl;

    // Main loop
    while (appletMainLoop())
    {
        
    }

    Logger::Close();

    /*
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface* screenSurface = NULL;

    int done = 0, x = 0, w = 1920, h = 1080;
    
    // mandatory at least on switch, else gfx is not properly closed
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        printf("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    // create an SDL window (OpenGL ES2 always enabled)
    // when SDL_FULLSCREEN flag is not set, viewport is automatically handled by SDL (use SDL_SetWindowSize to "change resolution")
    // available switch SDL2 video modes :
    // 1920 x 1080 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
    // 1280 x 720 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
    window = SDL_CreateWindow("sdl2_gles2", 0, 0, 1920, 1080, 0);
    if (!window) {
        printf("SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    //Get window surface
	screenSurface = SDL_GetWindowSurface( window );

	//Fill the surface white
	SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0x00, 0x00 ) );
			
	//Update the surface
	SDL_UpdateWindowSurface( window );

	//Wait two seconds
    printf("test");
	SDL_Delay( 2000 );
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
    */
}
