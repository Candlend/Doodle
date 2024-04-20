#pragma once

#ifdef DOO_PLATFORM_WINDOWS
#ifdef DOO_BUILD_DLL
#define DOO_API __declspec(dllexport)
#else
#define DOO_API __declspec(dllimport)
#endif
#else
#error Doodle only supports Windows!
#endif

#ifdef DOO_ENABLE_ASSERTS
	#define DOO_ASSERT(x, ...) { if(!(x)) { DOO_ERROR(__VA_ARGS__); __debugbreak(); } }
	#define DOO_CORE_ASSERT(x, ...) { if(!(x)) { DOO_CORE_ERROR(+ __VA_ARGS__); __debugbreak(); } }
#else
	#define DOO_ASSERT(x, ...)
	#define DOO_CORE_ASSERT(x, ...)
#endif
