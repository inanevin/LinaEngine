#include "Rendering/RenderBuffer.hpp"

#include "Core/RenderEngine.hpp"

namespace Lina::Graphics
{
    RenderBuffer::~RenderBuffer()
    {
        if (m_id != 0)
            m_id = RenderEngine::Get()->GetRenderDevice()->ReleaseRenderBufferObject(m_id);
    }

    void RenderBuffer::Construct(RenderBufferStorage storage, const Vector2i& size, int sampleCount)
    {
        m_id = RenderEngine::Get()->GetRenderDevice()->CreateRenderBufferObject(storage, (uint32)size.x, (uint32)size.y, sampleCount);
    }
} // namespace Lina::Graphics
