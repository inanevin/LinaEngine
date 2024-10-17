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

	struct ShaderProperty
	{
		ShaderPropertyType type = ShaderPropertyType::Bool;
		String			   name = "";
		Span<uint8>		   data;
		StringID		   sid = 0;

		ShaderProperty(){};
		ShaderProperty(const ShaderProperty& other)		 = delete;
		ShaderProperty& operator=(ShaderProperty const&) = delete;

		static bool VerifySimilarity(const Vector<ShaderProperty*>& v1, const Vector<ShaderProperty*>& v2);

		~ShaderProperty()
		{
			if (data.data() != nullptr)
				delete[] data.data();
			data = {};
		}

		void SaveToStream(OStream& out) const;
		void LoadFromStream(IStream& in);
	};

	enum class ShaderType
	{
		OpaqueSurface,
		TransparentSurface,
		Sky,
		PostProcess,
        Lighting,
		Custom
	};

	class Shader : public Resource
	{
	public:
		struct Metadata
		{
			HashMap<StringID, ShaderVariant> variants;
			ShaderType						 shaderType = ShaderType::Custom;

			void SaveToStream(OStream& out) const;
			void LoadFromStream(IStream& in);
		};

	public:
		static constexpr uint32 VERSION = 0;

		Shader(ResourceID id, const String& name) : Resource(id, GetTypeID<Shader>(), name){};
		virtual ~Shader();

		virtual bool LoadFromFile(const String& path) override;
		virtual void SaveToStream(OStream& stream) const override;
		virtual void LoadFromStream(IStream& stream) override;
		virtual void SetCustomMeta(IStream& stream) override
		{
			m_meta.LoadFromStream(stream);
		}

		void					GenerateHW();
		void					DestroyHW();
		void					Bind(LinaGX::CommandStream* stream, uint32 gpuHandle);
		void					AllocateDescriptorSet(DescriptorSet*& outSet, uint32& outIndex);
		void					FreeDescriptorSet(DescriptorSet* set, uint32 index);
		Vector<ShaderProperty*> CopyProperties();

		inline uint32 GetGPUHandle() const
		{
			return m_meta.variants.begin()->second._gpuHandle;
		}

		inline const LinaGX::DescriptorSetDesc& GetMaterialSetDesc() const
		{
			return m_materialSetDesc;
		}

		inline const LinaGX::ShaderDescriptorSetLayout GetMaterialSetInfo() const
		{
			return m_materialSetInfo;
		}

		inline const Metadata& GetMeta() const
		{
			return m_meta;
		}

		inline Metadata& GetMeta()
		{
			return m_meta;
		}

		inline const Vector<ShaderProperty*>& GetProperties() const
		{
			return m_properties;
		}

	private:
		ALLOCATOR_BUCKET_MEM;
		LINA_REFLECTION_ACCESS(Shader);

		LinaGX::DescriptorSetDesc		  m_materialSetDesc = {};
		LinaGX::ShaderDescriptorSetLayout m_materialSetInfo = {};
		Metadata						  m_meta			= {};
		Vector<DescriptorSet*>			  m_descriptorSets;
		Vector<ShaderProperty*>			  m_properties;
	};

	LINA_RESOURCE_BEGIN(Shader);
	LINA_FIELD(Shader, m_meta, "Metadata", FieldType::UserClass, GetTypeID<Shader::Metadata>())
	LINA_CLASS_END(Shader);

	LINA_CLASS_BEGIN(ShaderMetadata)
	LINA_CLASS_END(ShaderMetadata)
} // namespace Lina
