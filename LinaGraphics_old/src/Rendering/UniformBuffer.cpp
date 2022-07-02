#include "Rendering/UniformBuffer.hpp"

#include "Core/RenderEngine.hpp"

namespace Lina::Graphics
{
    UniformBuffer::~UniformBuffer()
    {
        if (m_isConstructed)
            m_engineBoundID = m_renderDevice->ReleaseUniformBuffer(m_engineBoundID);
    }

    // Updates the uniform buffer w/ new data, allows dynamic size change.
    void UniformBuffer::Construct(uintptr dataSize, BufferUsage usage, const void* data)
    {
        m_renderDevice  = RenderEngine::Get()->GetRenderDevice();
        m_bufferSize    = dataSize;
        m_engineBoundID = m_renderDevice->CreateUniformBuffer(data, dataSize, usage);
        m_isConstructed = true;
    }

    void UniformBuffer::Bind(uint32 point)
    {
        m_renderDevice->BindUniformBuffer(m_engineBoundID, point);
    }

    void UniformBuffer::Update(const void* data, uintptr offset, uintptr dataSize)
    {
        m_renderDevice->UpdateUniformBuffer(m_engineBoundID, data, offset, dataSize);
    }

    void UniformBuffer::Update(const void* data, uintptr dataSize)
    {
        m_renderDevice->UpdateUniformBuffer(m_engineBoundID, data, dataSize);
    }

} // namespace Lina::Graphics
