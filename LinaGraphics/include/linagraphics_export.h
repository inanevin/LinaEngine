
#ifndef LINAGRAPHICS_EXPORT_H
#define LINAGRAPHICS_EXPORT_H

#ifdef LINAGRAPHICS_STATIC_DEFINE
#  define LINAGRAPHICS_EXPORT
#  define LINAGRAPHICS_NO_EXPORT
#else
#  ifndef LINAGRAPHICS_EXPORT
#    ifdef LinaGraphics_EXPORTS
        /* We are building this library */
#      define LINAGRAPHICS_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINAGRAPHICS_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef LINAGRAPHICS_NO_EXPORT
#    define LINAGRAPHICS_NO_EXPORT 
#  endif
#endif

#ifndef LINAGRAPHICS_DEPRECATED
#  define LINAGRAPHICS_DEPRECATED __declspec(deprecated)
#endif

#ifndef LINAGRAPHICS_DEPRECATED_EXPORT
#  define LINAGRAPHICS_DEPRECATED_EXPORT LINAGRAPHICS_EXPORT LINAGRAPHICS_DEPRECATED
#endif

#ifndef LINAGRAPHICS_DEPRECATED_NO_EXPORT
#  define LINAGRAPHICS_DEPRECATED_NO_EXPORT LINAGRAPHICS_NO_EXPORT LINAGRAPHICS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LINAGRAPHICS_NO_DEPRECATED
#    define LINAGRAPHICS_NO_DEPRECATED
#  endif
#endif

#endif /* LINAGRAPHICS_EXPORT_H */
