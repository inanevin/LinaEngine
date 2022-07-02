/*
Class: UniformBuffer

Represents a uniform buffer structure in the shaders. It creates and updates the buffer data.

Timestamp: 4/26/2019 6:41:58 PM
*/

#pragma once

#ifndef UniformBuffer_HPP
#define UniformBuffer_HPP

#include "Core/RenderEngineFwd.hpp"
#include "RenderingCommon.hpp"

namespace Lina::Graphics
{
    class UniformBuffer
    {
    public:
        UniformBuffer()
        {
        }
        ~UniformBuffer();

        void   Construct(uintptr dataSize, BufferUsage usage, const void* data = nullptr);
        void   Bind(uint32 point);
        void   Update(const void* data, uintptr offset, uintptr dataSize);
        void   Update(const void* data, uintptr dataSize);
        uint32 GetID()
        {
            return m_engineBoundID;
        }

    private:
        RenderDevice* m_renderDevice  = nullptr;
        uint32        m_engineBoundID = 0;
        uintptr       m_bufferSize    = 0;
        bool          m_isConstructed = false;
    };
} // namespace Lina::Graphics

#endif
