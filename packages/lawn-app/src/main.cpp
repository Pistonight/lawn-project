#include "LawnApp.h"
// PISTON_PATCH
// #include "Resources.h"
#include "Sexy.TodLib/TodStringFile.h"
// PISTON_PATCH use WinMain to hide terminal
#include <windows.h>

using namespace Sexy;

bool (*gAppCloseRequest)();
bool (*gAppHasUsedCheatKeys)();
SexyString (*gGetCurrentLevelName)();

//0x44E8F0
// PISTON_PATCH use WinMain to hide terminal
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // PISTON_PATCH
	// Attach to parent console when launched from a terminal
	if (AttachConsole(ATTACH_PARENT_PROCESS)) {
		FILE* dummy;
		freopen_s(&dummy, "CONOUT$", "w", stdout);
		freopen_s(&dummy, "CONOUT$", "w", stderr);
	}

	TodStringListSetColors(gLawnStringFormats, gLawnStringFormatCount);
	gGetCurrentLevelName = LawnGetCurrentLevelName;
	gAppCloseRequest = LawnGetCloseRequest;
	gAppHasUsedCheatKeys = LawnHasUsedCheatKeys;

	gLawnApp = new LawnApp();
	gLawnApp->mChangeDirTo =
		(!Sexy::FileExists("properties/resources.xml") && Sexy::FileExists("../properties/resources.xml")) ? ".." : ".";

	gLawnApp->Init();
	gLawnApp->Start();

	gLawnApp->Shutdown();
	if (gLawnApp)
		delete gLawnApp;

	return 0;
};
