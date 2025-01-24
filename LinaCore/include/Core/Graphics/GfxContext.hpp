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

#pragma once

#include "Core/Resources/ResourceManagerListener.hpp"
#include "Core/Graphics/ResourceUploadQueue.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/MeshManager.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include <LinaGX/Common/CommonGfx.hpp>

namespace LinaGX
{
	class Instance;
	class CommandStream;
} // namespace LinaGX

namespace Lina
{
	class Texture;
	class TextureSampler;
	class Entity;
	class Material;
	class ResourceManagerV2;
	class GUIBackend;
	class Application;

	class GfxContext : public ResourceManagerListener
	{
	public:
		GfxContext()  = default;
		~GfxContext() = default;

		struct PerFrameData
		{
			LinaGX::CommandStream* copyStream	 = nullptr;
			SemaphoreData		   copySemaphore = {};

			uint16							  descriptorSetGlobal = 0;
			LinaGX::DescriptorUpdateImageDesc globalTexturesDesc  = {};
			LinaGX::DescriptorUpdateImageDesc globalSamplersDesc  = {};

			Buffer globalMaterialsBuffer = {};
			Buffer globalDataBuffer		 = {};
			bool   bindlessDirty		 = false;
		};

		void Initialize(Application* app);
		void Shutdown();
		void PollUploads(uint32 frameIndex);
		void UpdateBindless(uint32 frameIndex);
		void PrepareBindless();

		virtual void OnResourceManagerPreDestroyHW(const HashSet<Resource*>& resources) override;
		virtual void OnResourceManagerGeneratedHW(const HashSet<Resource*>& resources) override;
		void		 MarkBindlessDirty();

		inline uint16 GetDescriptorSetGlobal(uint32 frameIndex)
		{
			return m_pfd[frameIndex].descriptorSetGlobal;
		}

		inline ResourceUploadQueue& GetUploadQueue()
		{
			return m_uploadQueue;
		}

		inline uint16 GetPipelineLayoutGlobal() const
		{
			return m_pipelineLayoutGlobal;
		}

		inline uint16 GetPipelineLayoutPersistent(RenderPassType type) const
		{
			return m_pipelineLayoutPersistent[(uint8)type];
		}

		inline MeshManager& GetMeshManagerDefault()
		{
			return m_meshManagerDefault;
		}

		inline GUIBackend* GetGUIBackend() const
		{
			return m_guiBackend;
		}

	private:
		Application*		m_app		 = nullptr;
		ResourceManagerV2*	m_rm		 = nullptr;
		LinaGX::Instance*	m_lgx		 = nullptr;
		GUIBackend*			m_guiBackend = nullptr;
		ResourceUploadQueue m_uploadQueue;
		PerFrameData		m_pfd[FRAMES_IN_FLIGHT];
		uint16				m_pipelineLayoutGlobal = 0;
		Vector<uint16>		m_pipelineLayoutPersistent;

		MeshManager m_meshManagerDefault;

		Vector<Texture*>		m_nextTextureUpdates;
		Vector<TextureSampler*> m_nextSamplerUpdates;
		Vector<Material*>		m_nextMaterialUpdates;
		Atomic<bool>			m_bindlessDirty = false;
	};
} // namespace Lina
