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
#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/ClassMacros.hpp"
#include "Core/Graphics/Resource/Shader.hpp"

namespace Lina
{
	class Shader;
	class DescriptorSet;
	class ResourceManagerV2;
	class GfxContext;

	// Version changes
	// 0: initial

	struct MaterialProperty
	{
		ShaderPropertyDefinition propDef;
		Span<uint8>				 data;

		MaterialProperty() {};
		MaterialProperty(const MaterialProperty& other)		 = delete;
		MaterialProperty& operator=(MaterialProperty const&) = delete;

		~MaterialProperty()
		{
			if (data.data() != nullptr)
				delete[] data.data();
			data = {};
		}

		void SaveToStream(OStream& stream) const
		{
			stream << propDef;
			stream << static_cast<uint32>(data.size());
			stream.WriteRaw(data);
		}

		void LoadFromStream(IStream& stream)
		{
			stream >> propDef;

			uint32 size = 0;
			stream >> size;

			const size_t sz = static_cast<size_t>(size);
			data			= {new uint8[sz], sz};
			stream.ReadToRaw(data);
		}
	};

	class Material : public Resource
	{

	public:
		static constexpr uint32 VERSION = 0;

		Material(ResourceID id, const String& name) : Resource(id, GetTypeID<Material>(), name) {};
		virtual ~Material();
		virtual bool   LoadFromFile(const String& path) override;
		virtual void   SaveToStream(OStream& stream) const override;
		virtual void   LoadFromStream(IStream& stream) override;
		virtual size_t GetSize() const override;
		virtual void   GenerateHW() override;
		virtual void   DestroyHW() override;
		void		   SetShader(Shader* shader);
		size_t		   GetBufferSize();
		void		   BufferDataInto(Buffer& buf, size_t padding, ResourceManagerV2* rm, GfxContext* context);
		void		   CopyPropertiesFrom(Material* mat);

		template <typename T> void SetProperty(StringID sid, T val)
		{
			auto it = linatl::find_if(m_properties.begin(), m_properties.end(), [sid](MaterialProperty* p) -> bool { return p->propDef.sid == sid; });
			LINA_ASSERT(it != m_properties.end(), "Property not found!");
			MaterialProperty* prop = *it;
			LINA_ASSERT(sizeof(T) <= prop->data.size(), "");
			MEMCPY(prop->data.data(), &val, sizeof(T));
		}

		template <typename T> bool HasProperty(StringID sid)
		{
			auto it = linatl::find_if(m_properties.begin(), m_properties.end(), [sid](MaterialProperty* p) -> bool { return p->propDef.sid == sid; });
			return it != m_properties.end();
		}

		template <typename T> T* GetProperty(StringID sid)
		{
			auto			  it = linatl::find_if(m_properties.begin(), m_properties.end(), [sid](MaterialProperty* p) -> bool { return p->propDef.sid == sid; });
			MaterialProperty* p	 = (*it);
			return reinterpret_cast<T*>(p->data.data());
		}

		inline ResourceID GetShader() const
		{
			return m_shader;
		}

		inline const Vector<MaterialProperty*>& GetProperties() const
		{
			return m_properties;
		}

		inline ShaderType GetShaderType() const
		{
			return m_shaderType;
		}

		inline void SetShaderID(ResourceID id)
		{
			m_shader = id;
		}

	private:
		void DestroyProperties();

	private:
		ALLOCATOR_BUCKET_MEM;
		LINA_REFLECTION_ACCESS(Material);

		ResourceID				  m_shader	   = 0;
		ShaderType				  m_shaderType = ShaderType::Custom;
		Vector<MaterialProperty*> m_properties;
	};

	LINA_RESOURCE_BEGIN(Material);
	LINA_FIELD(Material, m_shader, "Shader", FieldType::ResourceID, GetTypeID<Shader>())
	LINA_CLASS_END(Material);

} // namespace Lina
