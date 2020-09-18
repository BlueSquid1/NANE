#include "CustomEventMgr.h"

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

std::unique_ptr<SDL_Event> CustomEventMgr::GenerateEvent(Uint32 eventName)
{
    std::unique_ptr<SDL_Event> generatedEvent = std::make_unique<SDL_Event>();
    generatedEvent->type = this->customEventType;
    generatedEvent->user.code = eventName;
    return generatedEvent;
}

Uint32 CustomEventMgr::GetCustomEventType()
{
    return this->customEventType;
}