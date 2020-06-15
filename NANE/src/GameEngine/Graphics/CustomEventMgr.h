#ifndef CUSTOM_EVENT_MANAGER
#define CUSTOM_EVENT_MANAGER

#include <memory>
#include <SDL_stdinc.h>

class CustomEventMgr
{
    private:
    static std::unique_ptr<CustomEventMgr> instance;

    Uint32 customEventType;
    
    //private constructor
    CustomEventMgr();

    public:
    static std::unique_ptr<CustomEventMgr>& GetInstance();

    //getters/setters
    Uint32 GetCustomEventType();
};


#endif