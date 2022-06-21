/*
Class: Memory

Memory manager wrapper.

Timestamp: 4/8/2019 9:04:58 PM

*/

#pragma once

#ifndef Memory_HPP
#define Memory_HPP

#define NOMINMAX

#include "Core/SizeDefinitions.hpp"

#include <cstring>

#define GENERIC_MEMORY_SMALL_MEMSWAP_MAX 16

namespace Lina
{

    struct Memory
    {
        enum
        {
            DefaultAlignment = 16,
            MinAlignment     = 8
        };

        static void* memmove(void* dest, const void* src, uintptr amt)
        {
            return ::memmove(dest, src, amt);
        }

        static int32 memcmp(const void* dest, const void* src, uintptr amt)
        {
            return ::memcmp(dest, src, amt);
        }

        template <typename T> static inline void* memset(void* destIn, T val, uintptr amt)
        {
            T*      dest      = (T*)destIn;
            uintptr amtT      = amt / sizeof(T);
            uintptr remainder = amt % sizeof(T);

            for (uintptr i = 0; i < amtT; ++i, ++dest)
                memcpy(dest, &val, sizeof(T));

            memcpy(dest, &val, remainder);
            return destIn;
        }

        static void* memzero(void* dest, uintptr amt)
        {
            return ::memset(dest, 0, amt);
        }

        static void* memcpy(void* dest, const void* src, uintptr amt)
        {
            return ::memcpy(dest, src, amt);
        }

        static void memswap(void* a, void* b, uintptr size)
        {
            if (size <= GENERIC_MEMORY_SMALL_MEMSWAP_MAX)
                smallmemswap(a, b, size);
            else
                bigmemswap(a, b, size);
        }

        template <typename T> static constexpr T align(const T ptr, uintptr alignment)
        {
            return (T)(((intptr)ptr + alignment - 1) & ~(alignment - 1));
        }

        static void*   malloc(uintptr amt, uint32 alignment = DefaultAlignment);
        static void*   realloc(void* ptr, uintptr amt, uint32 alignment);
        static void*   free(void* ptr);
        static uintptr getAllocSize(void* ptr);

    private:
        static void bigmemswap(void* a, void* b, uintptr size);
        static void smallmemswap(void* a, void* b, uintptr size)
        {
            // LINA_ASSERT(size <= GENERIC_MEMORY_SMALL_MEMSWAP_MAX, "Size is bigger than allowed!");
            char  temp_data[GENERIC_MEMORY_SMALL_MEMSWAP_MAX];
            void* temp = (void*)&temp_data;
            Memory::memcpy(temp, a, size);
            Memory::memcpy(a, b, size);
            Memory::memcpy(b, temp, size);
        }
    };

    template <> inline void* Memory::memset(void* dest, uint8 val, uintptr amt)
    {
        return ::memset(dest, val, amt);
    }
} // namespace Lina

#endif
