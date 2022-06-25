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


#pragma once

#ifndef Profiler_HPP
#define Profiler_HPP

// Headers here.

#ifdef LINA_ENABLE_PROFILING

#else

#endif

namespace Lina
{

#ifdef LINA_ENABLE_PROFILING

// #define PROFILER_MAIN_THREAD       EASY_MAIN_THREAD
// #define PROFILER_ENABLE            EASY_PROFILER_ENABLE
// #define PROFILER_BLOCK(...)        EASY_BLOCK(__VA_ARGS__)
// #define PROFILER_EVENT(...)        EASY_EVENT(__VA_ARGS__)
// #define PROFILER_THREAD(...)       EASY_THREAD(__VA_ARGS__)
// #define PROFILER_THREAD_SCOPE(...) EASY_THREAD_SCOPE(__VA_ARGS__)
// #define PROFILER_FUNC(...)         EASY_FUNCTION(__VA_ARGS__)
// #define PROFILER_STARTLISTEN       profiler::startListen()
// #define PROFILER_DUMP(...)         profiler::dumpBlocksToFile(__VA_ARGS__)
// #define PROFILER_END_BLOCK         EASY_END_BLOCK

#define PROFILER_MAIN_THREAD
#define PROFILER_ENABLE
#define PROFILER_BLOCK(...)
#define PROFILER_FUNC(...)
#define PROFILER_THREAD(...)
#define PROFILER_THREAD_SCOPE(...)
#define PROFILER_EVENT(...)
#define PROFILER_VALUE(...)
#define PROFILER_STARTLISTEN
#define PROFILER_DUMP(...)
#define PROFILER_END_BLOCK
#else
#define PROFILER_MAIN_THREAD
#define PROFILER_ENABLE
#define PROFILER_BLOCK(...)
#define PROFILER_FUNC(...)
#define PROFILER_THREAD(...)
#define PROFILER_THREAD_SCOPE(...)
#define PROFILER_EVENT(...)
#define PROFILER_VALUE(...)
#define PROFILER_STARTLISTEN
#define PROFILER_DUMP(...)

#endif
} // namespace Lina

#endif
