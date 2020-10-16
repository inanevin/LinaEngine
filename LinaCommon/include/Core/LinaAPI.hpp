/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
Class: LinaArray

Used for DLL exports.

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

