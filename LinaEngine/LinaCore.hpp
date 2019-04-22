
#ifndef LINA_API_H
#define LINA_API_H

#ifdef LINAENGINE_STATIC_DEFINE
#  define LINA_API
#  define LINAENGINE_NO_EXPORT
#else
#  ifndef LINA_API
#    ifdef LinaEngine_EXPORTS
        /* We are building this library */
#      define LINA_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINA_API __declspec(dllimport)
#    endif
#  endif

#  ifndef LINAENGINE_NO_EXPORT
#    define LINAENGINE_NO_EXPORT 
#  endif
#endif

#ifndef LINAENGINE_DEPRECATED
#  define LINAENGINE_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef LINAENGINE_DEPRECATED_EXPORT
#  define LINAENGINE_DEPRECATED_EXPORT LINA_API LINAENGINE_DEPRECATED
#endif

#ifndef LINAENGINE_DEPRECATED_NO_EXPORT
#  define LINAENGINE_DEPRECATED_NO_EXPORT LINAENGINE_NO_EXPORT LINAENGINE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LINAENGINE_NO_DEPRECATED
#    define LINAENGINE_NO_DEPRECATED
#  endif
#endif

#endif /* LINA_API_H */
