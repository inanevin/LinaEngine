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
#include "Graphics/Core/LGXWrapper.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "System/ISystem.hpp"
#include "Serialization/StringSerialization.hpp"
#include "FileSystem//FileSystem.hpp"

namespace Lina
{
	Shader::~Shader()
	{
		auto lgxWrapper = m_resourceManager->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper);
		auto lgx		= lgxWrapper->GetLGX();
		lgx->DestroyShader(m_gpuHandle);
	}

	String ExtractBlock(const String& source, const String& startDelimiter, const String& endDelimiter)
	{
		size_t startPos = source.find(startDelimiter);
		if (startPos == String::npos)
		{
			return ""; // Start delimiter not found
		}

		startPos += startDelimiter.length(); // Move past the delimiter
		size_t endPos = source.find(endDelimiter, startPos);
		if (endPos == String::npos)
		{
			return ""; // End delimiter not found
		}

		return source.substr(startPos, endPos - startPos);
	}

	void Shader::LoadFromFile(const char* path)
	{
		auto													   lgxWrapper = m_resourceManager->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper);
		auto													   lgx		  = lgxWrapper->GetLGX();
		LINAGX_MAP<LinaGX::ShaderStage, LinaGX::ShaderCompileData> data;
		LINAGX_MAP<LinaGX::ShaderStage, String>					   blocks;

		const String txt		 = FileSystem::ReadFileContentsAsString(path);
		const String includePath = FileSystem::GetFilePath(path);

		if (txt.find("#LINA_NOBLEND") != String::npos)
			m_meta.disableBlend = true;

		if (txt.find("#LINA_NODEPTH") != String::npos)
			m_meta.disableDepth = true;

		if (txt.find("#LINA_FINAL_PASS") != String::npos)
			m_meta.isFinalPass = true;

		HashMap<LinaGX::ShaderStage, String> blockIdentifiers;
		blockIdentifiers[LinaGX::ShaderStage::Fragment] = "#LINA_FS";
		blockIdentifiers[LinaGX::ShaderStage::Vertex]	= "#LINA_VS";
		blockIdentifiers[LinaGX::ShaderStage::Compute]	= "#LINA_CS";

		for (const auto& [stage, ident] : blockIdentifiers)
		{
			String					  block = ExtractBlock(txt.c_str(), ident.c_str(), "#LINA_END");
			LinaGX::ShaderCompileData compileData;

			if (!block.empty())
			{
				blocks[stage]			= block;
				compileData.includePath = includePath.c_str();
				compileData.text		= blocks[stage].c_str();
				data[stage]				= compileData;
			}
		}
		lgx->CompileShader(data, m_outCompiledBlobs, m_layout);
	}

	void Shader::SaveToStream(OStream& stream)
	{
		stream << m_meta.disableBlend << m_meta.disableDepth;

		const uint32 size = static_cast<uint32>(m_outCompiledBlobs.size());
		stream << size;

		for (const auto& [stage, blob] : m_outCompiledBlobs)
		{
			const uint8 stg = static_cast<uint8>(stage);
			stream << stg;
			stream << blob.size;
			stream.WriteEndianSafe(blob.ptr, blob.size);
		}

		SaveLinaGXShaderLayout(stream, m_layout);
	}

	void Shader::LoadFromStream(IStream& stream)
	{
		stream >> m_meta.disableBlend >> m_meta.disableDepth;

		uint32 size = 0;
		stream >> size;

		for (uint32 i = 0; i < size; i++)
		{
			uint8 stg = 0;
			stream >> stg;
			LinaGX::DataBlob blob;
			stream >> blob.size;

			if (blob.size != 0)
			{
				blob.ptr = new uint8[blob.size];
				stream.ReadEndianSafe(blob.ptr, blob.size);
			}

			m_outCompiledBlobs[static_cast<LinaGX::ShaderStage>(stg)] = blob;
		}

		m_layout = {};
		LoadLinaGXShaderLayout(stream, m_layout);
	}

	void Shader::BatchLoaded()
	{
		for (const auto& ubo : m_layout.ubos)
		{
			if (ubo.name.compare("LINA_MATERIAL") != 0)
			{
				m_materialUBO = ubo;
				break;
			}
		}
		auto lgxWrapper = m_resourceManager->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper);
		auto lgx		= lgxWrapper->GetLGX();

		LinaGX::ColorBlendAttachment blend = LinaGX::ColorBlendAttachment{
			.blendEnabled		 = !m_meta.disableBlend,
			.srcColorBlendFactor = LinaGX::BlendFactor::SrcAlpha,
			.dstColorBlendFactor = LinaGX::BlendFactor::OneMinusSrcAlpha,
			.colorBlendOp		 = LinaGX::BlendOp::Add,
			.srcAlphaBlendFactor = LinaGX::BlendFactor::One,
			.dstAlphaBlendFactor = LinaGX::BlendFactor::Zero,
			.alphaBlendOp		 = LinaGX::BlendOp::Add,
			.componentFlags		 = LinaGX::ColorComponentFlags::RGBA,
		};

		LinaGX::ShaderDesc desc = LinaGX::ShaderDesc{
			.stages				   = m_outCompiledBlobs,
			.colorAttachmentFormat = m_meta.isFinalPass ? LinaGX::Format::B8G8R8A8_UNORM : LinaGX::Format::R32G32B32A32_SFLOAT,
			.depthAttachmentFormat = LinaGX::Format::D32_SFLOAT,
			.layout				   = m_layout,
			.polygonMode		   = LinaGX::PolygonMode::Fill,
			.cullMode			   = LinaGX::CullMode::Back,
			.frontFace			   = LinaGX::FrontFace::CCW,
			.depthTest			   = !m_meta.disableDepth,
			.depthWrite			   = !m_meta.disableDepth,
			.depthCompare		   = LinaGX::CompareOp::Less,
			.topology			   = LinaGX::Topology::TriangleList,
			.blendAttachment	   = blend,
			.debugName			   = m_path.c_str(),
		};

		m_gpuHandle = lgx->CreateShader(desc);

		for (auto& [stage, blob] : m_outCompiledBlobs)
		{
			delete[] blob.ptr;
		}
	}

	void Shader::Flush()
	{
	}

} // namespace Lina
