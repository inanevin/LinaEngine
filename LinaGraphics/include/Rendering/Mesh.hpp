/*
Class: Mesh

Holds vertex, index, joint etc. data for model files.

Timestamp: 4/26/2019 12:11:36 AM
*/

#pragma once

#ifndef Mesh_HPP
#define Mesh_HPP

#include "Rendering/RenderingCommon.hpp"
#include "Math/AABB.hpp"
#include "Rendering/VertexArray.hpp"

namespace Lina::Graphics
{
    class Mesh
    {
    public:
        Mesh()
        {
        }
        virtual ~Mesh() = default;

        // Creates a vertex array using render render device.
        void CreateVertexArray(BufferUsage bufferUsage);

        // Sets the element size array according to the desired size.
        void AllocateElement(uint32 elementSize, uint32 attrib, bool isFloat, bool isInstanced = false);

        void AddElement(uint32 elementIndex, float e0);
        void AddElement(uint32 elementIndex, int e0);

        template <typename T, typename... Args> void AddElement(uint32 elementIndex, T first, Args... args)
        {
            AddElement(elementIndex, first);
            AddElement(elementIndex, args...);
        }

        void SetMaterialSlot(uint32 slotIndex)
        {
            m_materialSlot = slotIndex;
        }

        uint32 GetMaterialSlotIndex()
        {
            return m_materialSlot;
        }

        void SetName(const String& name)
        {
            m_name = name;
        }

        String& GetName()
        {
            return m_name;
        }

        void          AddIndices(uint32 i0, uint32 i1, uint32 i2);
        inline uint32 GetIndexCount() const
        {
            return (uint32)m_indices.size();
        }
        inline VertexArray& GetVertexArray()
        {
            return m_vertexArray;
        }
        inline AABB& GetAABB()
        {
            return m_aabb;
        }
      
        inline Vector3 GetVertexCenter()
        {
            return m_vertexCenter;
        }
        inline BufferData& GetVertexPositions()
        {
            return m_bufferElements[0];
        }

    protected:
        friend class ModelLoader;

        String             m_name = "";
        Vector<uint32>     m_indices;
        Vector<BufferData> m_bufferElements;
        VertexArray             m_vertexArray;
        uint32                  m_materialSlot      = 0;
        Vector3                 m_vertexCenter      = Vector3(0.0f, 0.0f, 0.0f);
        AABB                    m_aabb;
    };
} // namespace Lina::Graphics

#endif
