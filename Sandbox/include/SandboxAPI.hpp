/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#ifndef SANDBOX_API_H
#define SANDBOX_API_H

#ifdef SANDBOX_STATIC_DEFINE
#define SANDBOX_API
#define SANDBOX_NO_EXPORT
#else
#ifndef SANDBOX_API
#ifdef Sandbox_EXPORTS
/* We are building this library */
#define SANDBOX_API __declspec(dllexport)
#else
/* We are using this library */
#define SANDBOX_API __declspec(dllimport)
#endif
#endif

#ifndef SANDBOX_NO_EXPORT
#define SANDBOX_NO_EXPORT
#endif
#endif

#ifndef SANDBOX_DEPRECATED
#define SANDBOX_DEPRECATED __declspec(deprecated)
#endif

#ifndef SANDBOX_DEPRECATED_EXPORT
#define SANDBOX_DEPRECATED_EXPORT SANDBOX_API SANDBOX_DEPRECATED
#endif

#ifndef SANDBOX_DEPRECATED_NO_EXPORT
#define SANDBOX_DEPRECATED_NO_EXPORT SANDBOX_NO_EXPORT SANDBOX_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#ifndef SANDBOX_NO_DEPRECATED
#define SANDBOX_NO_DEPRECATED
#endif
#endif

#endif /* SANDBOX_API_H */
