
#ifndef LINARESOURCE_EXPORT_H
#define LINARESOURCE_EXPORT_H

#ifdef LINARESOURCE_STATIC_DEFINE
#  define LINARESOURCE_EXPORT
#  define LINARESOURCE_NO_EXPORT
#else
#  ifndef LINARESOURCE_EXPORT
#    ifdef LinaResource_EXPORTS
        /* We are building this library */
#      define LINARESOURCE_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINARESOURCE_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef LINARESOURCE_NO_EXPORT
#    define LINARESOURCE_NO_EXPORT 
#  endif
#endif

#ifndef LINARESOURCE_DEPRECATED
#  define LINARESOURCE_DEPRECATED __declspec(deprecated)
#endif

#ifndef LINARESOURCE_DEPRECATED_EXPORT
#  define LINARESOURCE_DEPRECATED_EXPORT LINARESOURCE_EXPORT LINARESOURCE_DEPRECATED
#endif

#ifndef LINARESOURCE_DEPRECATED_NO_EXPORT
#  define LINARESOURCE_DEPRECATED_NO_EXPORT LINARESOURCE_NO_EXPORT LINARESOURCE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LINARESOURCE_NO_DEPRECATED
#    define LINARESOURCE_NO_DEPRECATED
#  endif
#endif

#endif /* LINARESOURCE_EXPORT_H */
