/*
Class: VertexArray

Used for creation and storage of VAO data in the GPU backend.

Timestamp: 4/26/2019 12:30:15 AM
*/

#pragma once

#ifndef VertexArray_HPP
#define VertexArray_HPP

#include "Core/RenderEngineFwd.hpp"
#include "Core/SizeDefinitions.hpp"

namespace Lina::Graphics
{
    class VertexArray
    {
    public:
        VertexArray() : m_engineBoundID(0), m_indexCount(0), m_renderDevice(nullptr){};
        ~VertexArray();

        void   Initialize(uint32 engineBoundID, uint32 indexCount);
        void   UpdateBuffer(uint32 bufferIndex, const void* data, uintptr dataSize);
        uint32 GetID()
        {
            return m_engineBoundID;
        }
        uint32 GetIndexCount()
        {
            return m_indexCount;
        }

    private:
        RenderDevice* m_renderDevice  = nullptr;
        uint32        m_engineBoundID = 0;
        uint32        m_indexCount    = 0;
    };

} // namespace Lina::Graphics

#endif
