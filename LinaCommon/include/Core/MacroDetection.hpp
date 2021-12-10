#ifndef MACRODEF_HPP
#define MACRODEF_HPP

#ifdef _WIN32
	#ifdef _WIN64
		#define LINA_WINDOWS
	#else
		#error "x86 Builds are not supported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define LINA_IOS
	#elif TARGET_OS_MAC == 1
		#define LINA_MACOS
	#else
		#error "Unknown Apple platform!"
	#endif
#elif defined(__ANDROID__)
	#define LINA_ANDROID
#elif defined(__linux__)
	#define LINA_LINUX
#else
	#define	LINA_UNKNOWN_PLATFORM
#endif

#ifdef NDEBUG
	#define LINA_NONDEBUG_BUILD
#else
	#define LINA_DEBUG_BUILD
#endif

#endif
