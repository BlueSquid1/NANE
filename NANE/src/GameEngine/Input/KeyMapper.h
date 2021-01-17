#pragma once

#include <unordered_map>
#include <SDL.h> //SDL_Event

#include "NES/Controller/NesController.h"

class KeyMapper
{
    std::unordered_map<SDL_Keycode, NesController::NesInputs> inputMap = 
    {
        {SDLK_z, NesController::NesInputs::A},
        {SDLK_x, NesController::NesInputs::B},
        {SDLK_TAB, NesController::NesInputs::SELECT},
        {SDLK_RETURN, NesController::NesInputs::START},
        {SDLK_UP, NesController::NesInputs::UP},
        {SDLK_DOWN, NesController::NesInputs::DOWN},
        {SDLK_LEFT, NesController::NesInputs::LEFT},
        {SDLK_RIGHT, NesController::NesInputs::RIGHT}
    };

    public:
    NesController::NesInputs ToNesInput(SDL_Keycode& inputKey);
};