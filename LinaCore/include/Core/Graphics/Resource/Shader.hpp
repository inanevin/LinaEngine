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

#ifndef Shader_HPP
#define Shader_HPP

#include "Core/Resources/Resource.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Data/Streams.hpp"
#include "Common/Data/HashMap.hpp"
#include "Common/Data/Queue.hpp"
#include "ShaderVariant.hpp"
#include <variant>

namespace LinaGX
{
	class Instance;
}

namespace Lina
{
	class DescriptorSet;
	class GfxManager;

	struct ShaderProperty
	{
		ShaderPropertyType																									type = ShaderPropertyType::Bool;
		String																												name = "";
		size_t																												size = 0;
		std::variant<bool, float, uint32, int32, Vector2, Vector3, Vector4, Vector2ui, Vector3ui, Vector4ui, LinaTexture2D> data;
		StringID																											sid = 0;

		void SaveToStream(OStream& out) const;
		void LoadFromStream(IStream& in);
	};

	class Shader : public Resource
	{
	public:
		struct Metadata
		{
			HashMap<StringID, ShaderVariant> variants;
			RenderPassDescriptorType		 renderPassDescriptorType	  = RenderPassDescriptorType::Gui;
			uint32							 descriptorSetAllocationCount = 1;
			uint32							 materialSize				  = 0;
			bool							 drawIndirectEnabled		  = false;

			void SaveToStream(OStream& out) const;
			void LoadFromStream(IStream& in);
		};

	public:
		void Bind(LinaGX::CommandStream* stream, uint32 gpuHandle);
		void AllocateDescriptorSet(DescriptorSet*& outSet, uint32& outIndex);
		void FreeDescriptorSet(DescriptorSet* set, uint32 index);

		inline uint32 GetGPUHandle() const
		{
			return m_meta.variants.begin()->second._gpuHandle;
		}

		inline uint32 GetGPUHandle(StringID variant) const
		{
			return m_meta.variants.at(variant)._gpuHandle;
		}

		inline uint16 GetPipelineLayout() const
		{
			return m_pipelineLayout;
		}

		inline const LinaGX::DescriptorSetDesc& GetMaterialSetDesc() const
		{
			return m_materialSetDesc;
		}

		inline const LinaGX::ShaderDescriptorSetLayout GetMaterialSetInfo() const
		{
			return m_materialSetInfo;
		}

		inline const Metadata GetMeta() const
		{
			return m_meta;
		}

		inline const Vector<ShaderProperty>& GetProperties() const
		{
			return m_properties;
		}

	protected:
		virtual void LoadFromFile(const char* path) override;
		virtual void SaveToStream(OStream& stream) const override;
		virtual void LoadFromStream(IStream& stream) override;
		virtual void BatchLoaded() override;
		virtual void SetCustomMeta(IStream& stream) override
		{
			m_meta.LoadFromStream(stream);
		}

	private:
		FRIEND_RESOURCE_CACHE();
		Shader(ResourceManager* rm, const String& path, StringID sid);
		virtual ~Shader();

	private:
		LINAGX_MAP<LinaGX::ShaderStage, LinaGX::DataBlob> m_outCompiledBlobs;
		LinaGX::ShaderLayout							  m_layout			= {};
		LinaGX::DescriptorSetDesc						  m_materialSetDesc = {};
		LinaGX::ShaderDescriptorSetLayout				  m_materialSetInfo = {};
		Metadata										  m_meta			= {};
		uint16											  m_pipelineLayout;
		Vector<DescriptorSet*>							  m_descriptorSets;
		GfxManager*										  m_gfxManager = nullptr;
		LinaGX::Instance*								  m_lgx		   = nullptr;
		Vector<ShaderProperty>							  m_properties;
	};

} // namespace Lina

#endif
