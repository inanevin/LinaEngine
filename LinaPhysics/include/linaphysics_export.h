
#ifndef LINAPHYSICS_EXPORT_H
#define LINAPHYSICS_EXPORT_H

#ifdef LINAPHYSICS_STATIC_DEFINE
#  define LINAPHYSICS_EXPORT
#  define LINAPHYSICS_NO_EXPORT
#else
#  ifndef LINAPHYSICS_EXPORT
#    ifdef LinaPhysics_EXPORTS
        /* We are building this library */
#      define LINAPHYSICS_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINAPHYSICS_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef LINAPHYSICS_NO_EXPORT
#    define LINAPHYSICS_NO_EXPORT 
#  endif
#endif

#ifndef LINAPHYSICS_DEPRECATED
#  define LINAPHYSICS_DEPRECATED __declspec(deprecated)
#endif

#ifndef LINAPHYSICS_DEPRECATED_EXPORT
#  define LINAPHYSICS_DEPRECATED_EXPORT LINAPHYSICS_EXPORT LINAPHYSICS_DEPRECATED
#endif

#ifndef LINAPHYSICS_DEPRECATED_NO_EXPORT
#  define LINAPHYSICS_DEPRECATED_NO_EXPORT LINAPHYSICS_NO_EXPORT LINAPHYSICS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LINAPHYSICS_NO_DEPRECATED
#    define LINAPHYSICS_NO_DEPRECATED
#  endif
#endif

#endif /* LINAPHYSICS_EXPORT_H */
