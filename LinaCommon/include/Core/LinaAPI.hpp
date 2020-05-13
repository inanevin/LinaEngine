/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: LinaArray
Timestamp: 12/29/2018 10:43:46 PM

*/

#pragma once
#ifndef LINA_API_H
#define LINA_API_H



#ifdef LINACOMMON_STATIC_DEFINE
#define LINACOMMON_API
#else
#  ifndef LINACOMMON_API
#    ifdef LinaCommon_EXPORTS
        /* We are building this library */
#      define LINACOMMON_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINACOMMON_API __declspec(dllimport)
#    endif
#  endif
#  endif

#ifdef LINAACTION_STATIC_DEFINE
#define LINAACTION_API
#else
#  ifndef LINAACTION_API
#    ifdef LinaAction_EXPORTS
        /* We are building this library */
#      define LINAACTION_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINAACTION_API __declspec(dllimport)
#    endif
#  endif
#  endif

#ifdef LINAECS_STATIC_DEFINE
#define LINAECS_API
#else
#  ifndef LINAECS_API
#    ifdef LinaECS_EXPORTS
        /* We are building this library */
#      define LINAECS_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINAECS_API __declspec(dllimport)
#    endif
#  endif
#  endif

#ifdef LINAINPUT_STATIC_DEFINE
#define LINAINPUT_API
#else
#  ifndef LINAINPUT_API
#    ifdef LinaInput_EXPORTS
        /* We are building this library */
#      define LINAINPUT_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINAINPUT_API __declspec(dllimport)
#    endif
#  endif
#  endif

#ifdef LINAPHYSICS_STATIC_DEFINE
#define LINAPHYSICS_API
#else
#  ifndef LINAPHYSICS_API
#    ifdef LinaPhysics_EXPORTS
        /* We are building this library */
#      define LINAPHYSICS_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINAPHYSICS_API __declspec(dllimport)
#    endif
#  endif
#  endif

#ifdef LINAGRAPHICS_STATIC_DEFINE
#define LINAGRAPHICS_API
#else
#  ifndef LINAGRAPHICS_API
#    ifdef LinaGraphics_EXPORTS
        /* We are building this library */
#      define LINAGRAPHICS_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINAGRAPHICS_API __declspec(dllimport)
#    endif
#  endif
#  endif

#ifdef LINAENGINE_STATIC_DEFINE
#define LINAENGINE_API
#else
#  ifndef LINAENGINE_API
#    ifdef LinaEngine_EXPORTS
        /* We are building this library */
#      define LINAENGINE_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define LINAENGINE_API __declspec(dllimport)
#    endif
#  endif
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

