#include "CustomEventMgr.h"

#include <SDL.h>

std::unique_ptr<CustomEventMgr> CustomEventMgr::instance = NULL;

CustomEventMgr::CustomEventMgr()
{
    this->customEventType = SDL_RegisterEvents(1);
}

std::unique_ptr<CustomEventMgr>& CustomEventMgr::GetInstance()
{
    if(instance == NULL)
    {
        instance = std::unique_ptr<CustomEventMgr>(new CustomEventMgr());
    }
    return instance;
}

Uint32 CustomEventMgr::GetCustomEventType()
{
    return this->customEventType;
}