#include "Rendering/VertexArray.hpp"

#include "Core/RenderEngine.hpp"

namespace Lina::Graphics
{

    VertexArray::~VertexArray()
    {
        m_engineBoundID = m_renderDevice->ReleaseVertexArray(m_engineBoundID);
    }

    void VertexArray::UpdateBuffer(uint32 bufferIndex, const void* data, uintptr dataSize)
    {
        return m_renderDevice->UpdateVertexArrayBuffer(m_engineBoundID, bufferIndex, data, dataSize);
    }

    void VertexArray::Initialize(uint32 engineBoundID, uint32 indexCount)
    {
        m_renderDevice  = Graphics::RenderEngine::Get()->GetRenderDevice();
        m_engineBoundID = engineBoundID;
        m_indexCount    = indexCount;
    }

} // namespace Lina::Graphics
