#pragma once

// avoid rebuilding during development
#ifdef DEBUG
#define GIT_HASH "0000000000000000000000000000000000000000"
#define GIT_IS_DIRTY 1
#define GIT_AVAILABLE 1 

#define BUILD_NUMBER 999
#define VERSION_MAJOR 9
#define VERSION_MINOR 9
#define VERSION_PATCH 9

#define BUILD_DATE "2099/01/01 00:00:00"
#else
#include "./SexyAppFramework/BuildInfo.h"
#endif
