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
	class ResourceManager;
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
			void			 SaveToStream(OStream& stream) const;
			void			 LoadFromStream(IStream& stream);
		};

		Material(const String& path, StringID sid) : Resource(path, sid, GetTypeID<Material>()){};
		virtual ~Material();
		virtual void LoadFromFile(const char* path) override;
		virtual void SaveToStream(OStream& stream) const override;
		virtual void LoadFromStream(IStream& stream) override;
		void		 SetShader(Shader* shader);
        void ResetProperties();
		size_t		 BufferDataInto(Buffer& buf, size_t padding);

		template <typename T> void SetProperty(StringID sid, T val)
		{
			auto it = linatl::find_if(m_properties.begin(), m_properties.end(), [sid](const ShaderProperty& p) -> bool { return p.sid == sid; });
			LINA_ASSERT(it != m_properties.end(), "Property not found!");

			ShaderProperty& prop = *it;
			LINA_ASSERT(prop.size == sizeof(T), "Property size mismatch!");

			prop.data	 = val;
			m_propsDirty = true;
		}

		inline Shader* GetShader() const
		{
			return m_shader;
		}
        
        inline StringID GetShaderSID() const
        {
            return m_shaderSID;
        }

		inline uint32 GetBindlessBytePadding() const
		{
			return static_cast<uint32>(m_bindlessBytePadding);
		}

	private:
		ALLOCATOR_BUCKET_MEM;
		Shader*				   m_shader	   = nullptr;
		StringID			   m_shaderSID = 0;
		DescriptorAllocation   m_descriptorSetContainer[FRAMES_IN_FLIGHT];
		size_t				   m_bindlessBytePadding = 0;
		bool				   m_propsDirty			 = false;
		Vector<ShaderProperty> m_properties;
	};

	LINA_REFLECTRESOURCE_BEGIN(Material);
	LINA_REFLECTRESOURCE_END(Material);

} // namespace Lina
