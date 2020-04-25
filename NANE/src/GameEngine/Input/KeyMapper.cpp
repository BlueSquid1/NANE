#include "KeyMapper.h"


NesController::NesInputs KeyMapper::ToNesInput(SDL_Keycode& inputKey)
{
    if(this->inputMap.find(inputKey) == this->inputMap.end())
    {
        return NesController::NesInputs::UNDEFINED;
    }
    return this->inputMap.at(inputKey);
}