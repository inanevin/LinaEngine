
#ifndef GameCode_API_H
#define GameCode_API_H

#ifdef GAMECODE_STATIC_DEFINE
#  define GAMECODE_API
#  define GAMECODE_NO_EXPORT
#else
#  ifndef GAMECODE_API
#    ifdef GameCode_EXPORTS
        /* We are building this library */
#      define GAMECODE_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define GAMECODE_API __declspec(dllimport)
#    endif
#  endif

#  ifndef GAMECODE_NO_EXPORT
#    define GAMECODE_NO_EXPORT 
#  endif
#endif

#ifndef GAMECODE_DEPRECATED
#  define GAMECODE_DEPRECATED __declspec(deprecated)
#endif

#ifndef GAMECODE_DEPRECATED_EXPORT
#  define GAMECODE_DEPRECATED_EXPORT GameCode_API GAMECODE_DEPRECATED
#endif

#ifndef GAMECODE_DEPRECATED_NO_EXPORT
#  define GAMECODE_DEPRECATED_NO_EXPORT GAMECODE_NO_EXPORT GAMECODE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef GAMECODE_NO_DEPRECATED
#    define GAMECODE_NO_DEPRECATED
#  endif
#endif

#endif /* GameCode_API_H */
