/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Core/WorldRenderer.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Graphics/Core/ISwapchain.hpp"
#include "Graphics/Platform/RendererIncl.hpp"

namespace Lina
{
	int SurfaceRenderer::s_surfaceRendererCount = 0;

	SurfaceRenderer::SurfaceRenderer(GfxManager* man, uint32 imageCount, StringID sid, void* windowHandle, const Vector2i& initialSize, Bitmask16 mask)
		: m_gfxManager(man), m_imageCount(imageCount), m_sid(sid), m_windowHandle(windowHandle), m_size(initialSize), m_mask(mask)
	{
		m_gfxManager->GetSystem()->AddListener(this);
		m_swapchain = Renderer::CreateSwapchain(initialSize, windowHandle);

		if (m_mask.IsSet(SRM_DrawOffscreenTexture))
		{
			m_offscreenMaterials.resize(m_imageCount, nullptr);

			for (uint32 i = 0; i < m_imageCount; i++)
			{
				const String name		= "SurfaceRenderer_" + TO_STRING(s_surfaceRendererCount) + "OffscreenMat_" + TO_STRING(i);
				m_offscreenMaterials[i] = new Material(m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager), true, "", TO_SID(name));
				m_offscreenMaterials[i]->SetShader("Resources/Core/Shaders/ScreenQuads/SQFinal.linashader"_hs);
			}
		}
	}

	SurfaceRenderer::~SurfaceRenderer()
	{
		m_gfxManager->GetSystem()->RemoveListener(this);
		delete m_swapchain;

		if (m_mask.IsSet(SRM_DrawOffscreenTexture))
		{
			for (auto mat : m_offscreenMaterials)
				delete mat;
		}
	}

	void SurfaceRenderer::AddWorldRenderer(WorldRenderer* renderer)
	{
		m_worldRenderers.push_back(renderer);
	}

	void SurfaceRenderer::RemoveWorldRenderer(WorldRenderer* renderer)
	{
		m_worldRenderers.erase(linatl::find_if(m_worldRenderers.begin(), m_worldRenderers.end(), [renderer](WorldRenderer* r) { return r == renderer; }));
	}

	void SurfaceRenderer::SetOffscreenTexture(Texture* txt, uint32 imageIndex)
	{
		if (!m_mask.IsSet(SRM_DrawOffscreenTexture))
		{
			LINA_ERR("[Surface Renderer] -> Renderer is not set to draw an offscreen texture, returning!");
			return;
		}

		m_gfxManager->Join();

		m_offscreenMaterials[imageIndex]->SetTexture(0, txt->GetSID());
		m_offscreenMaterials[imageIndex]->UpdateBuffers(imageIndex);
	}

	void SurfaceRenderer::ClearOffscreenTexture()
	{
		if (!m_mask.IsSet(SRM_DrawOffscreenTexture))
		{
			LINA_ERR("[Surface Renderer] -> Renderer is not set to draw an offscreen texture, returning!");
			return;
		}

		m_gfxManager->Join();

		uint32 i = 0;
		for (auto mat : m_offscreenMaterials)
		{
			mat->SetTexture(0, "Resources/Core/Textures/LogoWithText.png"_hs);
			mat->UpdateBuffers(i);
			i++;
		}
	}

	void SurfaceRenderer::Tick(float delta)
	{
		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(m_worldRenderers.size()), 1, [&](int i) { m_worldRenderers[i]->Tick(delta); });
		m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);
	}

	void SurfaceRenderer::Render()
	{
	}

	void SurfaceRenderer::Join()
	{
		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(m_worldRenderers.size()), 1, [&](int i) { m_worldRenderers[i]->Join(); });
		m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);
	}

	void SurfaceRenderer::Present()
	{
	}

} // namespace Lina
