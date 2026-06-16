#pragma once

// only include generated build info to avoid rebuilding in debug mode
#ifdef DEBUG

#define GIT_HASH "debugdebugdebugdebugdebugdebugdebugdebug"
#define GIT_IS_DIRTY false
#define GIT_AVAILABLE true
#define BUILD_NUMBER 1
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#define BUILD_DATE "2099/09/09 21:09:09"

#else
#include "BuildInfo.inc"
#endif
