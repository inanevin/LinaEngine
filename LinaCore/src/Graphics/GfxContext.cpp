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

#include "Core/Graphics/GfxContext.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Resources/Resource.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/GUI/GUIBackend.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Application.hpp"
#include "Common/System/SystemInfo.hpp"

namespace Lina
{
	void GfxContext::Initialize(Application* app)
	{
		m_app		 = app;
		m_rm		 = &app->GetResourceManager();
		m_lgx		 = app->GetLGX();
		m_guiBackend = &app->GetGUIBackend();
		m_rm->AddListener(this);
		m_meshManagerDefault.Initialize();

		m_pipelineLayoutGlobal = m_lgx->CreatePipelineLayout(GfxHelpers::GetPLDescPersistentGlobal());

		m_pipelineLayoutPersistent.push_back(m_lgx->CreatePipelineLayout(GfxHelpers::GetPLDescPersistentRenderPass(RenderPassType::RENDER_PASS_DEFERRED)));
		m_pipelineLayoutPersistent.push_back(m_lgx->CreatePipelineLayout(GfxHelpers::GetPLDescPersistentRenderPass(RenderPassType::RENDER_PASS_FORWARD)));

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			data.descriptorSetGlobal = m_lgx->CreateDescriptorSet(GfxHelpers::GetSetDescPersistentGlobal());
			data.globalMaterialsBuffer.Create(LinaGX::ResourceTypeHint::TH_StorageBuffer, sizeof(uint32) * 1000, "Engine: Materials", false);
			data.globalDataBuffer.Create(LinaGX::ResourceTypeHint::TH_ConstantBuffer, sizeof(GPUDataEngineGlobals), "Engine: Globals", true);

			m_lgx->DescriptorUpdateBuffer({
				.setHandle	   = data.descriptorSetGlobal,
				.binding	   = 0,
				.buffers	   = {data.globalDataBuffer.GetGPUResource()},
				.isWriteAccess = false,
			});

			m_lgx->DescriptorUpdateBuffer({
				.setHandle	   = data.descriptorSetGlobal,
				.binding	   = 1,
				.buffers	   = {data.globalMaterialsBuffer.GetGPUResource()},
				.isWriteAccess = false,
			});

			data.globalTexturesDesc = {
				.setHandle = data.descriptorSetGlobal,
				.binding   = 2,
			};
			data.globalSamplersDesc = {
				.setHandle = data.descriptorSetGlobal,
				.binding   = 3,
			};

			data.copyStream	   = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, 50, 18000, 20000, 32, "Engine: Copy Stream"});
			data.copySemaphore = SemaphoreData(m_lgx->CreateUserSemaphore());
		}
	}

	void GfxContext::Shutdown()
	{
		m_meshManagerDefault.Shutdown();
		m_rm->RemoveListener(this);

		m_lgx->DestroyPipelineLayout(m_pipelineLayoutGlobal);

		for (uint16 l : m_pipelineLayoutPersistent)
			m_lgx->DestroyPipelineLayout(l);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			data.globalDataBuffer.Destroy();
			data.globalMaterialsBuffer.Destroy();
			m_lgx->DestroyDescriptorSet(data.descriptorSetGlobal);
			m_lgx->DestroyCommandStream(data.copyStream);
			m_lgx->DestroyUserSemaphore(data.copySemaphore.GetSemaphore());
			data.copyStream = nullptr;
		}
	}

	void GfxContext::MarkBindlessDirty()
	{
		m_bindlessDirty.store(true);
	}

	void GfxContext::PrepareBindless()
	{
		if (!m_bindlessDirty)
			return;

		m_bindlessDirty.store(false);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
			m_pfd[i].bindlessDirty = true;

		m_nextMaterialUpdates.resize(0);
		m_nextSamplerUpdates.resize(0);
		m_nextTextureUpdates.resize(0);

		// Textures
		ResourceCache<Texture>* cacheTxt = m_rm->GetCache<Texture>();
		cacheTxt->View([&](Texture* txt, uint32 index) -> bool {
			if (txt->GetIsMSAA())
				return false;

			Bitmask16 mask = txt->GetCreationFlags();
			if (!mask.IsSet(LinaGX::TF_Sampled))
				return false;

			const uint32 idx = static_cast<uint32>(m_nextTextureUpdates.size());
			txt->SetBindlessIndex(idx);
			m_nextTextureUpdates.push_back(txt);
			return false;
		});

		// Samplers
		ResourceCache<TextureSampler>* cacheSmp = m_rm->GetCache<TextureSampler>();
		cacheSmp->View([&](TextureSampler* smp, uint32 index) -> bool {
			m_nextSamplerUpdates.push_back(smp);
			smp->SetBindlessIndex(index);
			return false;
		});

		// Materials
		ResourceCache<Material>* cacheMat = m_rm->GetCache<Material>();
		size_t					 padding  = 0;
		cacheMat->View([&](Material* mat, uint32 index) -> bool {
			m_nextMaterialUpdates.push_back(mat);
			mat->SetBindlessIndex(static_cast<uint32>(padding) / static_cast<uint32>(sizeof(uint32)));
			padding += mat->GetBufferSize();
			return false;
		});
	}

	void GfxContext::UpdateBindless(uint32 frameIndex)
	{
		auto& pfd = m_pfd[frameIndex];

		if (!pfd.bindlessDirty)
			return;

		const size_t matSize = m_nextMaterialUpdates.size();
		const size_t smpSize = m_nextSamplerUpdates.size();
		const size_t txtSize = m_nextTextureUpdates.size();

		pfd.globalTexturesDesc.textures.resize(txtSize);
		pfd.globalSamplersDesc.samplers.resize(smpSize);

		for (size_t i = 0; i < txtSize; i++)
			pfd.globalTexturesDesc.textures[i] = m_nextTextureUpdates[i]->GetGPUHandle();

		if (!pfd.globalTexturesDesc.textures.empty())
			m_lgx->DescriptorUpdateImage(pfd.globalTexturesDesc);

		for (size_t i = 0; i < smpSize; i++)
			pfd.globalSamplersDesc.samplers[i] = m_nextSamplerUpdates[i]->GetGPUHandle();

		if (!pfd.globalSamplersDesc.samplers.empty())
			m_lgx->DescriptorUpdateImage(pfd.globalSamplersDesc);

		size_t padding = 0;

		for (size_t i = 0; i < matSize; i++)
		{
			Material* mat = m_nextMaterialUpdates.at(i);
			mat->BufferDataInto(pfd.globalMaterialsBuffer, padding, m_rm, this);
			padding += mat->GetBufferSize();
		}

		if (matSize != 0)
		{
			pfd.globalMaterialsBuffer.MarkDirty();
			m_uploadQueue.AddBufferRequest(&pfd.globalMaterialsBuffer);
		}

		pfd.bindlessDirty = false;
	}

	void GfxContext::PollUploads(uint32 frameIndex)
	{
		PerFrameData& pfd = m_pfd[frameIndex];

		// Update global data.
		{
			GPUDataEngineGlobals globalData = {};
			globalData.deltaElapsed			= Vector4(static_cast<float>(SystemInfo::GetDeltaTime()), SystemInfo::GetAppTimeF(), 0.0f, 0.0f);
			pfd.globalDataBuffer.BufferData(0, (uint8*)&globalData, sizeof(GPUDataEngineGlobals));
		}

		if (m_uploadQueue.FlushAll(pfd.copyStream))
		{
			pfd.copySemaphore.Increment();

			m_lgx->CloseCommandStreams(&pfd.copyStream, 1);

			m_lgx->SubmitCommandStreams({
				.targetQueue	  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Transfer),
				.streams		  = &pfd.copyStream,
				.streamCount	  = 1,
				.useSignal		  = true,
				.signalCount	  = 1,
				.signalSemaphores = pfd.copySemaphore.GetSemaphorePtr(),
				.signalValues	  = pfd.copySemaphore.GetValuePtr(),
			});
			m_lgx->WaitForUserSemaphore(pfd.copySemaphore.GetSemaphore(), pfd.copySemaphore.GetValue());
		}
	}

	void GfxContext::OnResourceManagerPreDestroyHW(const HashSet<Resource*>& resources)
	{
		bool bindlessDirty = false;
		bool containsFont  = false;
		bool join		   = false;
		bool containsMesh  = false;

		for (Resource* res : resources)
		{
			if (res->GetTID() == GetTypeID<Texture>())
			{
				join		  = true;
				bindlessDirty = true;

				m_uploadQueue.DestroyingTexture(static_cast<Texture*>(res));
			}
			else if (res->GetTID() == GetTypeID<TextureSampler>())
			{
				join		  = true;
				bindlessDirty = true;
			}
			else if (res->GetTID() == GetTypeID<Shader>())
			{
				join = true;
			}
			else if (res->GetTID() == GetTypeID<Material>())
			{
				join		  = true;
				bindlessDirty = true;
			}
			else if (res->GetTID() == GetTypeID<Model>())
			{
				join		 = true;
				containsMesh = true;
				static_cast<Model*>(res)->RemoveUpload(&m_meshManagerDefault);
			}
			else if (res->GetTID() == GetTypeID<Font>())
			{
				join		  = true;
				bindlessDirty = true;
				containsFont  = true;
			}
		}

		if (join)
			m_app->JoinRender();

		if (containsMesh)
		{
			m_meshManagerDefault.Refresh();
			m_meshManagerDefault.AddToUploadQueue(m_uploadQueue);
		}

		if (bindlessDirty)
			MarkBindlessDirty();

		if (containsFont)
			m_guiBackend->ReloadAtlases(m_uploadQueue);
	}

	void GfxContext::OnResourceManagerGeneratedHW(const HashSet<Resource*>& resources)
	{
		bool bindlessDirty = false;
		bool containsFont  = false;
		bool join		   = false;
		bool containsMesh  = false;

		for (Resource* res : resources)
		{
			if (res->GetTID() == GetTypeID<Texture>())
			{
				join		  = true;
				bindlessDirty = true;

				Texture* txt = static_cast<Texture*>(res);
				txt->AddToUploadQueue(m_uploadQueue, false);
			}
			else if (res->GetTID() == GetTypeID<TextureSampler>())
			{
				join		  = true;
				bindlessDirty = true;
			}
			else if (res->GetTID() == GetTypeID<Shader>())
			{
				join = true;
			}
			else if (res->GetTID() == GetTypeID<Material>())
			{
				join		  = true;
				bindlessDirty = true;
			}
			else if (res->GetTID() == GetTypeID<Model>())
			{
				join		 = true;
				containsMesh = true;
				static_cast<Model*>(res)->Upload(&m_meshManagerDefault);
			}
			else if (res->GetTID() == GetTypeID<Font>())
			{
				join		  = true;
				bindlessDirty = true;
				containsFont  = true;

				Font* font = static_cast<Font*>(res);
				font->Upload(m_guiBackend->GetLVGText());
			}
		}

		if (join)
			m_app->JoinRender();

		if (containsMesh)
			m_meshManagerDefault.AddToUploadQueue(m_uploadQueue);

		if (bindlessDirty)
			MarkBindlessDirty();

		if (containsFont)
			m_guiBackend->ReloadAtlases(m_uploadQueue);
	}

} // namespace Lina
