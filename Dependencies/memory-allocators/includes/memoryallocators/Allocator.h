#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef> // size_t

class Allocator
{
protected:
    std::size_t m_totalSize;
    std::size_t m_used;
    std::size_t m_peak;

public:
    Allocator(const std::size_t totalSize) : m_totalSize(totalSize), m_used(0), m_peak(0)
    {
    }

    virtual ~Allocator()
    {
        m_totalSize = 0;
    }

    virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) = 0;

    virtual void Free(void* ptr) = 0;

    virtual void Init() = 0;

    virtual void* GetStartPtr() = 0;

    inline std::size_t GetUsedSize() const
    {
        return m_used;
    }

    inline std::size_t GetTotalSize() const
    {
        return m_totalSize;
    }

    inline std::size_t GetPeak() const
    {
        return m_peak;
    }

    friend class Benchmark;
};

#endif /* ALLOCATOR_H */
