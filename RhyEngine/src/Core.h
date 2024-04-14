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

#ifdef RHY_ENABLE_ASSERTS
	#define RHY_ASSERT(x, ...) { if(!(x)) { RHY_ERROR(__VA_ARGS__); __debugbreak(); } }
	#define RHY_CORE_ASSERT(x, ...) { if(!(x)) { RHY_CORE_ERROR(+ __VA_ARGS__); __debugbreak(); } }
#else
	#define RHY_ASSERT(x, ...)
	#define RHY_CORE_ASSERT(x, ...)
#endif
