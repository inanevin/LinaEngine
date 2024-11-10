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

#include "Editor/Graphics/EditorRenderer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Graphics/SurfaceRenderer.hpp"
#include "Editor/Graphics/EditorGfxHelpers.hpp"
#include "Common/Data/CommonData.hpp"

#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Application.hpp"

namespace Lina::Editor
{
	void EditorRenderer::Initialize(Editor* editor)
	{
		m_editor			= editor;
		m_lgx				= Application::GetLGX();
		m_resourceManagerV2 = &editor->GetApp()->GetResourceManager();
		m_guiSampler		= m_resourceManagerV2->CreateResource<TextureSampler>(m_resourceManagerV2->ConsumeResourceID(), "EditorRendererGUISampler");
		m_guiTextSampler	= m_resourceManagerV2->CreateResource<TextureSampler>(m_resourceManagerV2->ConsumeResourceID(), "EditorRendererGUITextSampler");

		LinaGX::SamplerDesc samplerData = {};
		samplerData.minFilter			= LinaGX::Filter::Anisotropic;
		samplerData.magFilter			= LinaGX::Filter::Anisotropic;
		samplerData.mode				= LinaGX::SamplerAddressMode::Repeat;
		samplerData.anisotropy			= 6;
		samplerData.borderColor			= LinaGX::BorderColor::WhiteOpaque;
		samplerData.mipLodBias			= 0.0f;
		samplerData.minLod				= 0.0f;
		samplerData.maxLod				= 12;
		m_guiSampler->GenerateHW(samplerData);

		samplerData.magFilter = LinaGX::Filter::Anisotropic;
		m_guiTextSampler->GenerateHW(samplerData);

		m_pipelineLayoutGlobal = m_lgx->CreatePipelineLayout(EditorGfxHelpers::GetPipelineLayoutDescriptionGlobal());
		m_pipelineLayoutGUI	   = m_lgx->CreatePipelineLayout(EditorGfxHelpers::GetPipelineLayoutDescriptionGUI());
	}

	void EditorRenderer::Shutdown()
	{
		m_guiSampler->DestroyHW();
		m_guiTextSampler->DestroyHW();
		m_resourceManagerV2->DestroyResource(m_guiSampler);
		m_resourceManagerV2->DestroyResource(m_guiTextSampler);
		m_lgx->DestroyPipelineLayout(m_pipelineLayoutGUI);
		m_lgx->DestroyPipelineLayout(m_pipelineLayoutGlobal);
	}

	void EditorRenderer::PreTick()
	{
		m_validSurfaceRenderers.resize(0);

		// Main thread required.
		for (SurfaceRenderer* rend : m_surfaceRenderers)
		{
			if (rend->CheckVisibility())
				m_validSurfaceRenderers.push_back(rend);

			rend->PreTick();
		}

		/*
		if (m_validSurfaceRenderers.size() == 1)
		{
			m_validSurfaceRenderers[0]->PreTick();
		}
		else
		{
			Taskflow tf;
			tf.for_each_index(0, static_cast<int>(m_validSurfaceRenderers.size()), 1, [&](int i) {
				SurfaceRenderer* rend = m_validSurfaceRenderers.at(i);
				rend->PreTick();
			});
			m_editor->GetExecutor().RunAndWait(tf);
		}
		*/
	}

	void EditorRenderer::Tick(float delta)
	{
		if (!m_worldRenderers.empty())
		{
			if (m_worldRenderers.size() == 1)
				m_worldRenderers.at(0)->Tick(delta);
			else
			{
				Taskflow tf;
				tf.for_each_index(0, static_cast<int>(m_worldRenderers.size()), 1, [&](int i) {
					WorldRenderer* rend = m_worldRenderers.at(i);
					rend->Tick(delta);
				});
				m_executor.RunAndWait(tf);
			}
		}

		if (!m_validSurfaceRenderers.empty())
		{
			if (m_validSurfaceRenderers.size() == 1)
			{
				m_validSurfaceRenderers[0]->Tick(delta);
			}
			else
			{
				Taskflow tf;
				tf.for_each_index(0, static_cast<int>(m_validSurfaceRenderers.size()), 1, [&](int i) {
					SurfaceRenderer* rend = m_validSurfaceRenderers.at(i);
					rend->Tick(delta);
				});
				m_executor.RunAndWait(tf);
			}
		}
	}

	void EditorRenderer::SyncRender()
	{
		if (!m_worldRenderers.empty())
		{
			if (m_worldRenderers.size() == 1)
				m_worldRenderers.at(0)->SyncRender();
			else
			{
				Taskflow tf;
				tf.for_each_index(0, static_cast<int>(m_worldRenderers.size()), 1, [&](int i) {
					WorldRenderer* rend = m_worldRenderers.at(i);
					rend->SyncRender();
				});
				m_executor.RunAndWait(tf);
			}
		}

		if (!m_validSurfaceRenderers.empty())
		{
			if (m_validSurfaceRenderers.size() == 1)
			{
				m_validSurfaceRenderers[0]->SyncRender();
			}
			else
			{
				Taskflow tf;
				tf.for_each_index(0, static_cast<int>(m_validSurfaceRenderers.size()), 1, [&](int i) {
					SurfaceRenderer* rend = m_validSurfaceRenderers.at(i);
					rend->SyncRender();
				});
				m_executor.RunAndWait(tf);
			}
		}
	}

	void EditorRenderer::Render(uint32 frameIndex)
	{
		if (!m_worldRenderers.empty())
		{
			if (m_worldRenderers.size() == 1)
			{
				m_worldRenderers.at(0)->Render(frameIndex);
			}
			else
			{
				Taskflow tf;
				tf.for_each_index(0, static_cast<int>(m_worldRenderers.size()), 1, [&](int i) {
					WorldRenderer* rend = m_worldRenderers.at(i);
					rend->Render(frameIndex);
				});
				m_executor.RunAndWait(tf);
			}
		}

		Vector<uint16> waitSemaphores;
		Vector<uint64> waitValues;

		for (WorldRenderer* wr : m_worldRenderers)
		{
			const SemaphoreData sem = wr->GetSubmitSemaphore(frameIndex);
			waitSemaphores.push_back(sem.GetSemaphore());
			waitValues.push_back(sem.GetValue());
		}

		Vector<LinaGX::CommandStream*> streams;
		Vector<uint8>				   swapchains;
		streams.resize(m_validSurfaceRenderers.size());
		swapchains.resize(m_validSurfaceRenderers.size());

		if (m_validSurfaceRenderers.size() == 1)
		{
			SurfaceRenderer* rend = m_validSurfaceRenderers[0];
			streams[0]			  = rend->Render(frameIndex);
			swapchains[0]		  = rend->GetSwapchain();
		}
		else
		{
			Taskflow tf;
			tf.for_each_index(0, static_cast<int>(m_validSurfaceRenderers.size()), 1, [&](int i) {
				SurfaceRenderer* rend = m_validSurfaceRenderers.at(i);
				streams[i]			  = rend->Render(frameIndex);
				swapchains[i]		  = rend->GetSwapchain();
			});
			m_executor.RunAndWait(tf);
		}

		for (SurfaceRenderer* rend : m_validSurfaceRenderers)
		{
			const SemaphoreData& sem = rend->GetWaitSemaphore(frameIndex);
			if (sem.IsModified())
			{
				waitValues.push_back(sem.GetValue());
				waitSemaphores.push_back(sem.GetSemaphore());
			}
		}

		if (streams.empty())
			return;

		m_lgx->SubmitCommandStreams({
			.targetQueue	= m_lgx->GetPrimaryQueue(LinaGX::CommandType::Graphics),
			.streams		= streams.data(),
			.streamCount	= static_cast<uint32>(streams.size()),
			.useWait		= !waitValues.empty(),
			.waitCount		= static_cast<uint32>(waitValues.size()),
			.waitSemaphores = waitSemaphores.data(),
			.waitValues		= waitValues.data(),
			.useSignal		= false,
		});

		m_lgx->Present({
			.swapchains		= swapchains.data(),
			.swapchainCount = static_cast<uint32>(swapchains.size()),
		});
	}

	void EditorRenderer::AddWorldRenderer(WorldRenderer* wr)
	{
		m_worldRenderers.push_back(wr);
	}

	void EditorRenderer::RemoveWorldRenderer(WorldRenderer* wr)
	{
		m_worldRenderers.erase(linatl::find_if(m_worldRenderers.begin(), m_worldRenderers.end(), [wr](WorldRenderer* rend) -> bool { return wr == rend; }));
	}

	void EditorRenderer::AddSurfaceRenderer(SurfaceRenderer* sr)
	{
		m_surfaceRenderers.push_back(sr);
	}

	void EditorRenderer::RemoveSurfaceRenderer(SurfaceRenderer* sr)
	{
		m_surfaceRenderers.erase(linatl::find_if(m_surfaceRenderers.begin(), m_surfaceRenderers.end(), [sr](SurfaceRenderer* rend) -> bool { return sr == rend; }));
	}

} // namespace Lina::Editor
