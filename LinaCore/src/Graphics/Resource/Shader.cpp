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
#include "Graphics/Utility/ShaderPreprocessor.hpp"
#include "Serialization/HashMapSerialization.hpp"

namespace Lina
{
	Shader::~Shader()
	{
		auto lgxWrapper = m_resourceManager->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper);
		auto lgx		= lgxWrapper->GetLGX();

		for (const auto& [sid, var] : m_variants)
			lgx->DestroyShader(var.gpuHandle);
	}

	void Shader::LoadFromFile(const char* path)
	{
		auto													   lgxWrapper = m_resourceManager->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper);
		auto													   lgx		  = lgxWrapper->GetLGX();
		LINAGX_MAP<LinaGX::ShaderStage, LinaGX::ShaderCompileData> data;
		LINAGX_MAP<LinaGX::ShaderStage, String>					   blocks;

		String		 txt		 = FileSystem::ReadFileContentsAsString(path);
		const String includePath = FileSystem::GetFilePath(path);

		HashMap<LinaGX::ShaderStage, String> outStages;

		const bool success = ShaderPreprocessor::Preprocess(txt, outStages, m_variants);
		if (!success)
			return;

		for (const auto& [stg, text] : outStages)
		{
			LinaGX::ShaderCompileData compData;
			compData.includePath = includePath.c_str();
			compData.text		 = text.c_str();
			data[stg]			 = compData;
		}

		lgx->CompileShader(data, m_outCompiledBlobs, m_layout);
	}

	void Shader::SaveToStream(OStream& stream)
	{
		HashMapSerialization::SaveToStream_OBJ(stream, m_variants);

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
		HashMapSerialization::LoadFromStream_OBJ(stream, m_variants);

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
		auto lgxWrapper = m_resourceManager->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper);
		auto lgx		= lgxWrapper->GetLGX();

		for (auto& [sid, variant] : m_variants)
		{
			LinaGX::Format format = LinaGX::Format::B8G8R8A8_SRGB;

			if (variant.passType == ShaderVariantPassType::RenderTarget)
				format = LinaGX::Format::R32G32B32A32_SFLOAT;

			LinaGX::ColorBlendAttachment blend = LinaGX::ColorBlendAttachment{
				.blendEnabled		 = !variant.blendDisable,
				.srcColorBlendFactor = LinaGX::BlendFactor::SrcAlpha,
				.dstColorBlendFactor = LinaGX::BlendFactor::OneMinusSrcAlpha,
				.colorBlendOp		 = LinaGX::BlendOp::Add,
				.srcAlphaBlendFactor = LinaGX::BlendFactor::One,
				.dstAlphaBlendFactor = LinaGX::BlendFactor::Zero,
				.alphaBlendOp		 = LinaGX::BlendOp::Add,
				.componentFlags		 = {},
			};

			LINAGX_VEC<LinaGX::ShaderColorAttachment> colorAttachments;
			colorAttachments.resize(1);

			colorAttachments[0] = {
				.format			 = format,
				.blendAttachment = blend,
			};

			LinaGX::ShaderDepthStencilDesc depthStencilAtt = {
				.depthStencilAttachmentFormat = LinaGX::Format::D32_SFLOAT,
				.depthWrite					  = !variant.depthDisable,
				.depthTest					  = !variant.depthDisable,
				.depthCompare				  = LinaGX::CompareOp::Less,
			};

			LinaGX::ShaderDesc desc = LinaGX::ShaderDesc{
				.stages			  = m_outCompiledBlobs,
				.colorAttachments = colorAttachments,
				.depthStencilDesc = depthStencilAtt,
				.layout			  = m_layout,
				.polygonMode	  = LinaGX::PolygonMode::Fill,
				.cullMode		  = variant.cullMode,
				.frontFace		  = variant.frontFace,
				.topology		  = LinaGX::Topology::TriangleList,
				.debugName		  = m_path.c_str(),
			};

			variant.gpuHandle = lgx->CreateShader(desc);
		}

		for (auto& [stage, blob] : m_outCompiledBlobs)
		{
			delete[] blob.ptr;
		}
	}

	void Shader::Flush()
	{
	}

	void ShaderVariant::SaveToStream(OStream& stream)
	{
		const uint8 passTypeInt	 = static_cast<uint8>(passType);
		const uint8 cullModeInt	 = static_cast<uint8>(cullMode);
		const uint8 frontFaceInt = static_cast<uint8>(frontFace);
		stream << gpuHandle << blendDisable << depthDisable << passTypeInt << cullModeInt << frontFaceInt;
		StringSerialization::SaveToStream(stream, passName);
		StringSerialization::SaveToStream(stream, name);
	}

	void ShaderVariant::LoadFromStream(IStream& stream)
	{
		uint8 passTypeInt = 0, cullModeInt = 0, frontFaceInt = 0;
		stream >> gpuHandle >> blendDisable >> depthDisable >> passTypeInt >> cullModeInt >> frontFaceInt;
		StringSerialization::LoadFromStream(stream, passName);
		StringSerialization::LoadFromStream(stream, name);
		passType  = static_cast<ShaderVariantPassType>(passTypeInt);
		cullMode  = static_cast<LinaGX::CullMode>(cullModeInt);
		frontFace = static_cast<LinaGX::FrontFace>(frontFaceInt);
	}

} // namespace Lina
