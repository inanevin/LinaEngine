
#ifndef GamePlugin_API_H
#define GamePlugin_API_H

#ifdef GAMEPLUGIN_STATIC_DEFINE
#define GAMEPLUGIN_API
#define GAMEPLUGIN_NO_EXPORT
#else
#ifndef GAMEPLUGIN_API
#ifdef GamePlugin_EXPORTS
/* We are building this library */
#define GAMEPLUGIN_API __declspec(dllexport)
#else
/* We are using this library */
#define GAMEPLUGIN_API __declspec(dllimport)
#endif
#endif

#ifndef GAMEPLUGIN_NO_EXPORT
#define GAMEPLUGIN_NO_EXPORT
#endif
#endif

#ifndef GAMEPLUGIN_DEPRECATED
#define GAMEPLUGIN_DEPRECATED __declspec(deprecated)
#endif

#ifndef GAMEPLUGIN_DEPRECATED_EXPORT
#define GAMEPLUGIN_DEPRECATED_EXPORT GamePlugin_API GAMEPLUGIN_DEPRECATED
#endif

#ifndef GAMEPLUGIN_DEPRECATED_NO_EXPORT
#define GAMEPLUGIN_DEPRECATED_NO_EXPORT GAMEPLUGIN_NO_EXPORT GAMEPLUGIN_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#ifndef GAMEPLUGIN_NO_DEPRECATED
#define GAMEPLUGIN_NO_DEPRECATED
#endif
#endif

#endif /* GamePlugin_API_H */
