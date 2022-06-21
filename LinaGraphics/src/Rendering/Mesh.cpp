#include "Rendering/Mesh.hpp"

#include "Core/RenderEngine.hpp"

namespace Lina::Graphics
{
    void Mesh::AddElement(uint32 elementIndex, float e0)
    {
        if (m_bufferElements[elementIndex].m_isFloat)
            m_bufferElements[elementIndex].m_floatElements.push_back(e0);
        else
            m_bufferElements[elementIndex].m_intElements.push_back((int)e0);
    }

    void Mesh::AddElement(uint32 elementIndex, int e0)
    {

        if (m_bufferElements[elementIndex].m_isFloat)
            m_bufferElements[elementIndex].m_floatElements.push_back((float)e0);
        else
            m_bufferElements[elementIndex].m_intElements.push_back(e0);
    }

    void Mesh::AddIndices(uint32 i0, uint32 i1, uint32 i2)
    {
        m_indices.push_back(i0);
        m_indices.push_back(i1);
        m_indices.push_back(i2);
    }

    void Mesh::AllocateElement(uint32 elementSize, uint32 attrib, bool isFloat, bool isInstanced)
    {
        m_bufferElements.push_back(BufferData(elementSize, attrib, isFloat, isInstanced));
    }

    void Mesh::CreateVertexArray(BufferUsage bufferUsage)
    {
        uint32 numVertices = (uint32)m_bufferElements[0].m_floatElements.size() / m_bufferElements[0].m_elementSize;
        uint32 numIndices  = (uint32)m_indices.size();

        int totalVertexComponents = 0;
        for (int i = 0; i < m_bufferElements.size(); i++)
        {
            if (!m_bufferElements[i].m_isInstanced)
                totalVertexComponents++;
        }

        int totalInstanceComponents = (uint32)m_bufferElements.size() - totalVertexComponents;

        // Init vertex array.
        uint32 id = RenderEngine::Get()->GetRenderDevice()->CreateVertexArray(m_bufferElements, totalVertexComponents, totalInstanceComponents, numVertices, &m_indices[0], numIndices, bufferUsage);
        m_vertexArray.Initialize(id, GetIndexCount());
    }
} // namespace Lina::Graphics
