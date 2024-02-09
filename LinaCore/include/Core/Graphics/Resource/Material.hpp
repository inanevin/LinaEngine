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

#ifndef Material_HPP
#define Material_HPP

#include "Core/Resources/Resource.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/ClassMacros.hpp"

namespace LinaGX
{
	class Instance;
	class CommandStream;
} // namespace LinaGX

namespace Lina
{
	class MaterialPropertyBase;
	class Shader;
	class GfxManager;
	class DescriptorSet;

	class Material : public Resource
	{

	private:
		struct DescriptorAllocation
		{
			DescriptorSet* set		  = nullptr;
			uint32		   allocIndex = 0;
		};

	public:
		struct BindingBuffers
		{
			Vector<Buffer> buffers;
		};

		struct BindingData
		{
			BindingBuffers	 bufferData[FRAMES_IN_FLIGHT];
			Vector<StringID> textures;
			Vector<StringID> samplers;

			void SaveToStream(OStream& stream) const;
			void LoadFromStream(LinaGX::Instance* lgx, IStream& stream);
		};

		void SetShader(StringID sid);
		void Bind(LinaGX::CommandStream* stream, uint32 frameIndex, LinaGX::DescriptorSetsLayoutSource layoutSource, uint32 customShaderHandle = 0);
		void SetBuffer(uint32 bindingIndex, uint32 descriptorIndex, size_t padding, uint8* data, size_t dataSize);
		void SetTexture(uint32 bindingIndex, uint32 descriptorIndex, uint32 gpuHandle);
		void SetSampler(uint32 bindingIndex, uint32 descriptorIndex, uint32 gpuHandle);
		void SetCombinedImageSampler(uint32 bindingIndex, uint32 descriptorIndex, uint32 textureGPUHandle, uint32 samplerGPUHandle);

		inline Buffer& GetBuffer(uint32 bindingIndex, uint32 descriptorIndex, uint32 frameIndex)
		{
			return m_bindingData[bindingIndex].bufferData[frameIndex].buffers[descriptorIndex];
		}

	private:
		FRIEND_RESOURCE_CACHE();
		Material(ResourceManager* rm, const String& path, StringID sid);
		virtual ~Material();

	protected:
		virtual void LoadFromFile(const char* path) override;
		virtual void SaveToStream(OStream& stream) const override;
		virtual void LoadFromStream(IStream& stream) override;
		virtual void BatchLoaded() override;

	private:
		void CreateDescriptorSets();
		void DestroyDescriptorSets();
		void CreateBindingData();
		void DestroyBindingData();
		void UpdateBinding(uint32 bindingIndex);

	private:
		LinaGX::Instance*	 m_lgx		  = nullptr;
		GfxManager*			 m_gfxManager = nullptr;
		Shader*				 m_shader	  = nullptr;
		StringID			 m_shaderSID  = 0;
		DescriptorAllocation m_descriptorSetContainer[FRAMES_IN_FLIGHT];
		Vector<BindingData>	 m_bindingData;
	};

	DEFINE_CLASS_STORAGE_TYPE(Material);

} // namespace Lina

#endif