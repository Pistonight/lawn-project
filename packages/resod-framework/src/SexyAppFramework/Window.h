#pragma once

#include "Platform.h" // for Windows, do not remove
#include <SDL3/SDL.h>

namespace Sexy {
class SexyAppBase;

class Window {
public:
    SexyAppBase* mApp;
    SDL_Window* mInternalWindow;

public:
    Window(SexyAppBase* theApp);
    ~Window();

#ifdef _WIN32
    HWND GetHWND();
#endif
};

} // namespace Sexy
