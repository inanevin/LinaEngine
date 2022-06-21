#include "Memory/Memory.hpp"

#include "Math/Math.hpp"

#include <cstdlib>
#include <stdio.h>

namespace Lina
{

    void* Memory::malloc(uintptr amt, uint32 alignment)
    {
        alignment                                                       = Math::Max(amt >= 16 ? 16u : 8u, alignment);
        void* ptr                                                       = ::malloc(amt + alignment + sizeof(void*) + sizeof(uintptr));
        void* result                                                    = align((uint8*)ptr + sizeof(void*) + sizeof(uintptr), (uintptr)alignment);
        *((void**)((uint8*)result - sizeof(void*)))                     = ptr;
        *((uintptr*)((uint8*)result - sizeof(void*) - sizeof(uintptr))) = amt;
        return result;
    }

    void* Memory::realloc(void* ptr, uintptr amt, uint32 alignment)
    {
        alignment = Math::Max(amt >= 16 ? 16u : 8u, alignment);

        if (ptr == nullptr)
            return Memory::malloc(amt, alignment);

        if (amt == 0)
        {
            Memory::free(ptr);
            return nullptr;
        }

        void*   result = malloc(amt, alignment);
        uintptr size   = Memory::getAllocSize(ptr);
        Memory::memcpy(result, ptr, Math::Min(size, amt));
        free(ptr);

        return result;
    }

    void* Memory::free(void* ptr)
    {
        if (ptr)
            ::free(*((void**)((uint8*)ptr - sizeof(void*))));

        return nullptr;
    }

    uintptr Memory::getAllocSize(void* ptr)
    {
        return *((uintptr*)((uint8*)ptr - sizeof(void*) - sizeof(uintptr)));
    }

    void Memory::bigmemswap(void* a, void* b, uintptr size)
    {
        uint64* ptr1 = (uint64*)a;
        uint64* ptr2 = (uint64*)b;

        while (size > GENERIC_MEMORY_SMALL_MEMSWAP_MAX)
        {
            uint64 tmp = *ptr1;
            *ptr1      = *ptr2;
            *ptr2      = tmp;
            size -= 8;
            ptr1++;
            ptr2++;
        }

        smallmemswap(ptr1, ptr2, size);
    }
} // namespace Lina
