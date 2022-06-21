#include "Rendering/PostProcessEffect.hpp"

#include "Core/RenderEngine.hpp"
#include "Helpers/DrawParameterHelper.hpp"
#include "Rendering/RenderConstants.hpp"

namespace Lina::Graphics
{

    void PostProcessEffect::Construct(Shader* shader)
    {
        m_renderEngine                                        = RenderEngine::Get();
        m_renderDevice                                        = m_renderEngine->GetRenderDevice();
        m_samplerParams.m_textureParams.m_pixelFormat         = PixelFormat::FORMAT_RGB;
        m_samplerParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGBA16F;
        m_samplerParams.m_textureParams.m_minFilter = m_samplerParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
        m_samplerParams.m_textureParams.m_wrapS = m_samplerParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
        m_rtTexture.ConstructRTTexture(m_renderEngine->GetScreenSize(), m_samplerParams, false);
        m_renderTarget.Construct(m_rtTexture, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);
        m_drawParams = DrawParameterHelper::GetFullScreenQuad();
        m_material.SetShader(shader);
    }

    void PostProcessEffect::Draw(Texture* screenMap)
    {
        // Select FBO
        m_renderDevice->SetFBO(m_renderTarget.GetID());

        // Clear color bit.
        m_renderDevice->Clear(true, true, true, Color::White, 0xFF);

        // Setup material & use.
        m_material.SetTexture(MAT_MAP_SCREEN, screenMap);

        m_renderEngine->UpdateShaderData(&m_material);
        m_renderDevice->Draw(m_renderEngine->GetScreenQuadVAO(), m_drawParams, 0, 6, true);
    }
} // namespace Lina::Graphics
