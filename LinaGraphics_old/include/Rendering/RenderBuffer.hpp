/*
Class: RenderBuffer

Used for managing and storing render buffer data from the GPU backend.


Timestamp: 5/19/2020 8:10:11 PM
*/

#pragma once

#ifndef RenderBuffer_HPP
#define RenderBuffer_HPP

#include "Rendering/RenderingCommon.hpp"

namespace Lina::Graphics
{
    class RenderBuffer
    {

    public:
        RenderBuffer() = default;
        ~RenderBuffer();

        void Construct(RenderBufferStorage storage, const Vector2i& size, int sampleCount = 0);

        uint32 GetID()
        {
            return m_id;
        }

    private:
        uint32 m_id = 0;
    };
} // namespace Lina::Graphics

#endif
