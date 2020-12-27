
#ifndef LINACOMMON_EXPORT_H
#define LINACOMMON_EXPORT_H

#ifdef LINACOMMON_STATIC_DEFINE
#  define LINACOMMON_EXPORT
#  define LINACOMMON_NO_EXPORT
#else
#  ifndef LINACOMMON_EXPORT
#    ifdef LinaCommon_EXPORTS
        /* We are building this library */
#      define LINACOMMON_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINACOMMON_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef LINACOMMON_NO_EXPORT
#    define LINACOMMON_NO_EXPORT 
#  endif
#endif

#ifndef LINACOMMON_DEPRECATED
#  define LINACOMMON_DEPRECATED __declspec(deprecated)
#endif

#ifndef LINACOMMON_DEPRECATED_EXPORT
#  define LINACOMMON_DEPRECATED_EXPORT LINACOMMON_EXPORT LINACOMMON_DEPRECATED
#endif

#ifndef LINACOMMON_DEPRECATED_NO_EXPORT
#  define LINACOMMON_DEPRECATED_NO_EXPORT LINACOMMON_NO_EXPORT LINACOMMON_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LINACOMMON_NO_DEPRECATED
#    define LINACOMMON_NO_DEPRECATED
#  endif
#endif

#endif /* LINACOMMON_EXPORT_H */
