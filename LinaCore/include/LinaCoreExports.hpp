
#ifndef LINACORE_API_H
#define LINACORE_API_H

#ifdef LINACORE_STATIC_DEFINE
#  define LINACORE_API
#  define LINACORE_NO_EXPORT
#else
#  ifndef LINACORE_API
#    ifdef LinaCore_EXPORTS
        /* We are building this library */
#      define LINACORE_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINACORE_API __declspec(dllimport)
#    endif
#  endif

#  ifndef LINACORE_NO_EXPORT
#    define LINACORE_NO_EXPORT 
#  endif
#endif

#ifndef LINACORE_DEPRECATED
#  define LINACORE_DEPRECATED __declspec(deprecated)
#endif

#ifndef LINACORE_DEPRECATED_EXPORT
#  define LINACORE_DEPRECATED_EXPORT LINACORE_API LINACORE_DEPRECATED
#endif

#ifndef LINACORE_DEPRECATED_NO_EXPORT
#  define LINACORE_DEPRECATED_NO_EXPORT LINACORE_NO_EXPORT LINACORE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LINACORE_NO_DEPRECATED
#    define LINACORE_NO_DEPRECATED
#  endif
#endif

#endif /* LINACORE_API_H */
