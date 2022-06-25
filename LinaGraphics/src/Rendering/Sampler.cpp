#include "Rendering/Sampler.hpp"

#include "Core/RenderDevice.hpp"
#include "Core/RenderEngine.hpp"

namespace Lina::Graphics
{
    Sampler::~Sampler()
    {
        m_engineBoundID = m_renderDevice->ReleaseSampler(m_engineBoundID);
    }

    void Sampler::Construct(SamplerParameters samplerParams, TextureBindMode bindMode, bool isCubemap)
    {
        m_renderDevice   = RenderEngine::Get()->GetRenderDevice();
        m_engineBoundID  = m_renderDevice->CreateSampler(samplerParams, isCubemap);
        m_targetBindMode = bindMode;
        m_params         = samplerParams;
    }

    void Sampler::UpdateSettings(SamplerParameters samplerParams)
    {
        m_params = samplerParams;
        m_renderDevice->UpdateSamplerParameters(m_engineBoundID, samplerParams);
        m_renderDevice->UpdateTextureParameters(m_targetBindMode, m_targetTextureID, samplerParams);
    }

} // namespace Lina::Graphics
