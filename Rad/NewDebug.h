#pragma once
#include <crtdbg.h>

#ifdef _DEBUG
#define DEBUG_NEW_BLOCK(x) (x, __FILE__, __LINE__)
#else
#define DEBUG_NEW_BLOCK(x)
#endif

#define DEBUG_NEW DEBUG_NEW_BLOCK(_NORMAL_BLOCK)
