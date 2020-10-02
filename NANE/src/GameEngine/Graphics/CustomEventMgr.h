#ifndef CUSTOM_EVENT_MANAGER
#define CUSTOM_EVENT_MANAGER

#include <memory>
#include <SDL_events.h>

enum class CustomEvents : Uint32
{
    OpenRom,
    SimpleView,
    DisassembleView,
    ContinuePauseEmulator,
    StepEmulator,
    SaveSnapshot,
    LoadSnapshot,
    IncrementDefaultColourPalette,
    StepAssembly,
    ShowFpsCounter,
    ScaleFactor1,
    ScaleFactor2,
    ScaleFactor3,
    ScaleFactor4,
    ScaleFactor5,
    FileSelected, // when a file is selected during OpenRom window
    DumpProgramRom
};

class CustomEventMgr
{
    private:
    static std::unique_ptr<CustomEventMgr> instance;

    Uint32 customEventType;
    
    //private constructor
    CustomEventMgr();

    public:
    static std::unique_ptr<CustomEventMgr>& GetInstance();

    std::unique_ptr<SDL_Event> GenerateEvent(Uint32 eventName);

    //getters/setters
    Uint32 GetCustomEventType();
};


#endif