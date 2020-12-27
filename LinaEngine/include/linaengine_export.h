
#ifndef LINAENGINE_EXPORT_H
#define LINAENGINE_EXPORT_H

#ifdef LINAENGINE_STATIC_DEFINE
#  define LINAENGINE_EXPORT
#  define LINAENGINE_NO_EXPORT
#else
#  ifndef LINAENGINE_EXPORT
#    ifdef LinaEngine_EXPORTS
        /* We are building this library */
#      define LINAENGINE_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINAENGINE_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef LINAENGINE_NO_EXPORT
#    define LINAENGINE_NO_EXPORT 
#  endif
#endif

#ifndef LINAENGINE_DEPRECATED
#  define LINAENGINE_DEPRECATED __declspec(deprecated)
#endif

#ifndef LINAENGINE_DEPRECATED_EXPORT
#  define LINAENGINE_DEPRECATED_EXPORT LINAENGINE_EXPORT LINAENGINE_DEPRECATED
#endif

#ifndef LINAENGINE_DEPRECATED_NO_EXPORT
#  define LINAENGINE_DEPRECATED_NO_EXPORT LINAENGINE_NO_EXPORT LINAENGINE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LINAENGINE_NO_DEPRECATED
#    define LINAENGINE_NO_DEPRECATED
#  endif
#endif

#endif /* LINAENGINE_EXPORT_H */
