#pragma once

#ifdef RHY_PLATFORM_WINDOWS
#ifdef RHY_BUILD_DLL
#define RHY_API __declspec(dllexport)
#else
#define RHY_API __declspec(dllimport)
#endif
#else
#error RhyEngine only supports Windows!
#endif