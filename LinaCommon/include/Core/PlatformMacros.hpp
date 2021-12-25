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


#ifdef LINA_COMPILER_MSVC
#define FORCEINLINE __forceinline
#elif defined(LINA_COMPILER_GCC) || defined(LINA_COMPILER_CLANG)
#define FORCEINLINE inline __attribute__ ((always_inline))
#else
#define FORCEINLINE inline
#endif

#define BIT(x) (1 << x)
#define INVALID_VALUE 0xFFFFFFFF
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define LINA_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define NULL_COPY_AND_ASSIGN(T) \
	T(const T& other) {(void)other;} \
	void operator=(const T& other) { (void)other; }

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete;

#define DISALLOW_COPY_ASSIGN_MOVE(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete; \
	TypeName(TypeName&&) = delete; \
  TypeName& operator=(TypeName&&) = delete; 

#define DISALLOW_COPY_ASSIGN_NEW(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete; \
  void* operator new(std::size_t) = delete;

#define DISALLOW_COPY_ASSIGN_NEW_MOVE(TypeName) \
  TypeName(const TypeName&) = delete;      \
  TypeName(TypeName&&) = delete; \
  TypeName& operator=(TypeName&&) = delete; \
  void operator=(const TypeName&) = delete; \
  void* operator new(std::size_t) = delete;

#endif
