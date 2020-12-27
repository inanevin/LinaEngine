
#ifndef LINAAUDIO_EXPORT_H
#define LINAAUDIO_EXPORT_H

#ifdef LINAAUDIO_STATIC_DEFINE
#  define LINAAUDIO_EXPORT
#  define LINAAUDIO_NO_EXPORT
#else
#  ifndef LINAAUDIO_EXPORT
#    ifdef LinaAudio_EXPORTS
        /* We are building this library */
#      define LINAAUDIO_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINAAUDIO_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef LINAAUDIO_NO_EXPORT
#    define LINAAUDIO_NO_EXPORT 
#  endif
#endif

#ifndef LINAAUDIO_DEPRECATED
#  define LINAAUDIO_DEPRECATED __declspec(deprecated)
#endif

#ifndef LINAAUDIO_DEPRECATED_EXPORT
#  define LINAAUDIO_DEPRECATED_EXPORT LINAAUDIO_EXPORT LINAAUDIO_DEPRECATED
#endif

#ifndef LINAAUDIO_DEPRECATED_NO_EXPORT
#  define LINAAUDIO_DEPRECATED_NO_EXPORT LINAAUDIO_NO_EXPORT LINAAUDIO_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LINAAUDIO_NO_DEPRECATED
#    define LINAAUDIO_NO_DEPRECATED
#  endif
#endif

#endif /* LINAAUDIO_EXPORT_H */
