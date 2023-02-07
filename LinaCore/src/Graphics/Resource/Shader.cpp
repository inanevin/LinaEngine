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

#include "Graphics/Resource/Shader.hpp"
#include "Serialization/StringSerialization.hpp"
#include "Serialization/VectorSerialization.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Graphics/Utility/GLSLParser.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Platform/GfxManagerIncl.hpp"

#ifdef LINA_GRAPHICS_VULKAN
#include "Graphics/Platform/Vulkan/Utility/SPIRVUtility.hpp"
#endif

namespace Lina
{
	Shader::~Shader()
	{
		if (m_gpuHandle == -1)
			return;

		static_cast<GfxManager*>(m_resourceManager->GetSystem()->GetSubsystem(SubsystemType::GfxManager))->GetGPUStorage().DestroyPipeline(m_gpuHandle);

		for (auto p : m_properties)
			delete p;

		for (auto t : m_textures)
			delete t;

		for (auto& [stg, code] : m_compiledCode)
			code.clear();
	}

	Vector<unsigned int> Shader::GetCompiledCode(ShaderStage stage) const
	{
		return m_compiledCode.at(stage);
	}

	void Shader::LoadFromFile(const char* path)
	{
		m_text = FileSystem::ReadFileContentsAsString(path);

#ifdef LINA_GRAPHICS_VULKAN
		GLSLParser::ParseRaw(m_text, m_stages, m_materialBindings, m_properties, m_textures, m_drawPassMask, m_pipelineType);
		for (auto& [stage, text] : m_stages)
			SPIRVUtility::GLSLToSPV(stage, text.c_str(), m_compiledCode[stage]);
#else
		LINA_NOTIMPLEMENTED;
#endif
	}

	void Shader::SaveToStream(OStream& stream)
	{
		const uint8	 ppType			   = static_cast<uint8>(m_pipelineType);
		const uint16 drawPassMaskValue = m_drawPassMask.GetValue();
		stream << ppType << drawPassMaskValue;

		const uint32 stageSize = static_cast<uint32>(m_compiledCode.size());
		stream << stageSize;

		for (auto& [stage, code] : m_compiledCode)
		{
			const uint8 stg = static_cast<uint8>(stage);
			stream << stg;

			auto&		 code	  = m_compiledCode[static_cast<ShaderStage>(stage)];
			const uint32 codeSize = static_cast<uint32>(code.size());
			stream << codeSize;

			if (codeSize != 0)
			{
				uint8* ptr = (uint8*)&code[0];
				stream.WriteEndianSafe(ptr, codeSize * sizeof(unsigned int));
			}
		}

		const uint32 bindingsSize = static_cast<uint32>(m_materialBindings.size());
		stream << bindingsSize;

		for (auto& b : m_materialBindings)
		{
			uint8 typeInt = static_cast<uint8>(b.type);
			stream << b.binding;
			stream << b.descriptorCount;
			stream << typeInt;

			const uint32 bindingStagesSize = static_cast<uint32>(b.stages.size());
			stream << bindingStagesSize;

			for (auto s : b.stages)
			{
				const uint8 stage = static_cast<uint8>(s);
				stream << stage;
			}
		}

		const uint32 propertiesSize = static_cast<uint32>(m_properties.size());
		stream << propertiesSize;

		for (auto& p : m_properties)
		{
			const uint8 type = static_cast<uint8>(p->GetType());
			stream << type;
			StringSerialization::SaveToStream(stream, p->GetName());
			p->SaveToStream(stream);
		}

		const uint32 texturesSize = static_cast<uint32>(m_textures.size());
		stream << texturesSize;

		for (auto& t : m_textures)
		{
			StringSerialization::SaveToStream(stream, t->GetName());
			t->SaveToStream(stream);
		}
	}

	void Shader::LoadFromStream(IStream& stream)
	{
		uint8  ppType			 = 0;
		uint16 drawPassMaskValue = 0;
		stream >> ppType >> drawPassMaskValue;
		m_pipelineType = static_cast<PipelineType>(ppType);
		m_drawPassMask.Set(drawPassMaskValue);

		uint32 stageSize = 0;
		stream >> stageSize;

		for (uint32 i = 0; i < stageSize; i++)
		{
			uint8 stageI = 0;
			stream >> stageI;

			// no need for txt, only keep the map alive.
			m_stages[static_cast<ShaderStage>(stageI)] = "";

			auto&  code		= m_compiledCode[static_cast<ShaderStage>(stageI)];
			uint32 codeSize = 0;
			stream >> codeSize;

			if (codeSize != 0)
			{
				code.resize(codeSize);
				stream.ReadEndianSafe(&code[0], codeSize * sizeof(unsigned int));
			}
		}

		uint32 bindingsSize = 0;
		stream >> bindingsSize;

		for (uint32 i = 0; i < bindingsSize; i++)
		{
			UserBinding b;
			uint8		typeInt = 0;
			stream >> b.binding;
			stream >> b.descriptorCount;
			stream >> typeInt;
			b.type = static_cast<DescriptorType>(typeInt);

			uint32 bindingStagesSize = 0;
			stream >> bindingStagesSize;

			for (uint32 j = 0; j < bindingStagesSize; j++)
			{
				uint8 stage = 0;
				stream >> stage;
				b.stages.push_back(static_cast<ShaderStage>(stage));
			}

			m_materialBindings.push_back(b);
		}

		uint32 propertiesSize = 0;
		stream >> propertiesSize;

		for (uint32 i = 0; i < propertiesSize; i++)
		{
			uint8 type = 0;
			stream >> type;
			String name = "";
			StringSerialization::LoadFromStream(stream, name);
			MaterialPropertyBase* prop = MaterialPropertyBase::CreateProperty(static_cast<MaterialPropertyType>(type), name);
			prop->LoadFromStream(stream);
			m_properties.push_back(prop);
		}

		uint32 texturesSize = 0;
		stream >> texturesSize;

		for (uint32 i = 0; i < texturesSize; i++)
		{
			String name = "";
			StringSerialization::LoadFromStream(stream, name);
			MaterialPropertyBase* prop = MaterialPropertyBase::CreateProperty(MaterialPropertyType::Texture, name);
			prop->LoadFromStream(stream);
			m_textures.push_back(prop);
		}
	}

	void Shader::Upload()
	{
		if (m_gpuHandle != -1)
			return;

		m_gpuHandle = static_cast<GfxManager*>(m_resourceManager->GetSystem()->GetSubsystem(SubsystemType::GfxManager))->GetGPUStorage().GeneratePipeline(this);
	}

	void Shader::Flush()
	{
		// rest of the data is still needed by materials changing shaders etc.
		m_text.clear();
		m_compiledCode.clear();
	}
} // namespace Lina
