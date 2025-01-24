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
#include "Common/System/SystemInfo.hpp"

namespace Lina
{
	void GfxContext::Initialize(ResourceManagerV2* rm, LinaGX::Instance* lgx, GUIBackend* guiBackend)
	{
		m_rm = rm;
		m_rm->AddListener(this);
		m_lgx		 = lgx;
		m_guiBackend = guiBackend;

		m_pipelineLayoutGlobal = m_lgx->CreatePipelineLayout(GfxHelpers::GetPLDescPersistentGlobal());

		LinaGX::ResourceDesc globalDataDesc = {
			.size		   = sizeof(GPUDataEngineGlobals),
			.typeHintFlags = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
			.heapType	   = LinaGX::ResourceHeap::StagingHeap,
			.debugName	   = "WorldRenderer: Global Data Buffer",
		};

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			data.descriptorSetGlobal = m_lgx->CreateDescriptorSet(GfxHelpers::GetSetDescPersistentGlobal());
			data.globalMaterialsBuffer.Create(LinaGX::ResourceTypeHint::TH_StorageBuffer, sizeof(uint32) * 1000, "WorldRendererr: Materials", false);
			data.globalDataBuffer.Create(LinaGX::ResourceTypeHint::TH_ConstantBuffer, sizeof(GPUDataEngineGlobals), "WorldRenderer: Engine Globals", true);

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

			data.copyStream	   = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, 50, 12000, 4096, 32, "BindlessContext: Copy Stream"});
			data.copySemaphore = SemaphoreData(m_lgx->CreateUserSemaphore());
		}
	}

	void GfxContext::Shutdown()
	{

		m_rm->RemoveListener(this);

		m_lgx->DestroyPipelineLayout(m_pipelineLayoutGlobal);

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

	void GfxContext::UpdateBindless(uint32 frameIndex)
	{
		auto& pfd = m_pfd[frameIndex];

		if (!pfd.bindlessDirty)
			return;

		// Textures.
		ResourceCache<Texture>* cacheTxt	 = m_rm->GetCache<Texture>();
		const size_t			txtCacheSize = static_cast<size_t>(cacheTxt->GetActiveItemCount());
		pfd.globalTexturesDesc.textures.resize(txtCacheSize);
		cacheTxt->View([&](Texture* txt, uint32 index) -> bool {
			pfd.globalTexturesDesc.textures[index] = txt->GetGPUHandle();
			txt->SetBindlessIndex(index);
			return false;
		});

		if (!pfd.globalTexturesDesc.textures.empty())
			m_lgx->DescriptorUpdateImage(pfd.globalTexturesDesc);

		// Samplers
		ResourceCache<TextureSampler>* cacheSmp = m_rm->GetCache<TextureSampler>();
		pfd.globalSamplersDesc.samplers.resize(static_cast<size_t>(cacheSmp->GetActiveItemCount()));
		cacheSmp->View([&](TextureSampler* smp, uint32 index) -> bool {
			pfd.globalSamplersDesc.samplers[index] = smp->GetGPUHandle();
			smp->SetBindlessIndex(index);
			return false;
		});

		if (!pfd.globalSamplersDesc.samplers.empty())
			m_lgx->DescriptorUpdateImage(pfd.globalSamplersDesc);

		ResourceCache<Material>* cacheMat = m_rm->GetCache<Material>();
		size_t					 padding  = 0;
		cacheMat->View([&](Material* mat, uint32 index) -> bool {
			mat->SetBindlessIndex(static_cast<uint32>(padding));
			padding += mat->BufferDataInto(pfd.globalMaterialsBuffer, padding, m_rm, this);
			pfd.globalMaterialsBuffer.MarkDirty();
			return false;
		});

		if (padding != 0)
			m_uploadQueue.AddBufferRequest(&pfd.globalMaterialsBuffer);

		pfd.bindlessDirty = false;
	}

	void GfxContext::MarkBindlessDirty()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
			m_pfd[i].bindlessDirty = true;
	}

	void GfxContext::PollUploads(uint32 frameIndex)
	{
		PerFrameData& pfd = m_pfd[frameIndex];

		UpdateBindless(frameIndex);

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

		// Update global data.
		{
			const auto&			 mp			= m_lgx->GetInput().GetMousePositionAbs();
			GPUDataEngineGlobals globalData = {};
			globalData.mouse				= Vector4(static_cast<float>(mp.x), static_cast<float>(mp.y), 0.0f, 0.0f);
			globalData.deltaElapsed			= Vector4(static_cast<float>(SystemInfo::GetDeltaTime()), SystemInfo::GetAppTimeF(), 0.0f, 0.0f);
			pfd.globalDataBuffer.BufferData(0, (uint8*)&globalData, sizeof(GPUDataEngineGlobals));
		}
	}

	void GfxContext::OnResourceManagerPreDestroyHW(const HashSet<Resource*>& resources)
	{

		bool bindlessDirty = false;
		bool containsFont  = false;
		bool join		   = false;

		for (Resource* res : resources)
		{
			if (res->GetTID() == GetTypeID<Texture>())
			{
				join		  = true;
				bindlessDirty = true;
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
				join = true;
			}
			else if (res->GetTID() == GetTypeID<Font>())
			{
				join		  = true;
				bindlessDirty = true;
				containsFont  = true;
			}
		}

		if (join)
			m_lgx->Join();

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
				join = true;
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
			m_lgx->Join();

		if (bindlessDirty)
			MarkBindlessDirty();

		if (containsFont)
			m_guiBackend->ReloadAtlases(m_uploadQueue);
	}

} // namespace Lina
