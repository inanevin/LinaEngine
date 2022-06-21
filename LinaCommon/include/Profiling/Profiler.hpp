/*
Class: Profiler


Timestamp: 12/20/2020 12:28:41 AM
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
