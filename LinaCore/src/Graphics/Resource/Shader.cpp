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

#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Application.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Utility/ShaderPreprocessor.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/Pipeline/DescriptorSet.hpp"

#include "Common/FileSystem/FileSystem.hpp"
#include <LinaGX/Utility/SPIRVUtility.hpp>

namespace Lina
{
	Delegate<void(Shader*)> Shader::s_variantInjection = nullptr;

	Shader::~Shader()
	{
		ClearVariantCompileData();
	}

	uint32 Shader::GetGPUHandle(DepthTesting depthMode)
	{
		for (ShaderVariant& variant : m_meta.variants)
		{
			if (variant.depthMode == depthMode)
				return GetGPUHandle(variant.id);
		}
		LINA_ASSERT(false, "");
		return 0;
	}

	bool Shader::CompileVariants()
	{
		bool success = true;
		for (ShaderVariant& variant : m_meta.variants)
		{
			for (LinaGX::ShaderCompileData& data : variant._compileData)
			{
				String fullText = "";
				LinaGX::SPIRVUtility::GetShaderTextWithIncludes(fullText, data.text, data.includePath);
				data.text = fullText;
			}

			success = LinaGX::Instance::CompileShaderToSPV(variant._compileData, variant._outLayout);

			if (!success)
			{
				LINA_ERR("Failed compiling shader! {0}", m_name);
				break;
			}
		}

		return success;
	}

	void Shader::Bind(LinaGX::CommandStream* stream, uint32 gpuHandle)
	{
		LinaGX::CMDBindPipeline* bind = stream->AddCommand<LinaGX::CMDBindPipeline>();
		bind->shader				  = gpuHandle;
	}

	void Shader::ClearVariantCompileData()
	{
		for (ShaderVariant& variant : m_meta.variants)
		{
			for (LinaGX::ShaderCompileData& data : variant._compileData)
				delete[] data.outBlob.ptr;
		}
	}

	bool Shader::LoadFromFile(const String& path)
	{
		if (!FileSystem::FileOrPathExists(path))
			return false;

		const String includePath = FileSystem::GetRunningDirectory();
		if (includePath.empty())
			return false;

		const String txt = FileSystem::ReadFileContentsAsString(path);
		if (txt.empty())
			return false;

		if (!ShaderPreprocessor::VerifyFullShader(txt))
			return false;

		String vertexBlock = "", fragBlock = "";
		if (!ShaderPreprocessor::ExtractVertexFrag(txt, vertexBlock, fragBlock))
			return false;

		auto injectMaterials = [this](String& vertex, String& pixel) -> bool {
			Vector<ShaderPropertyDefinition> vertexProperties = {}, fragProperties = {};
			if (!ShaderPreprocessor::InjectMaterialIfRequired(vertex, vertexProperties))
				return false;

			if (!ShaderPreprocessor::InjectMaterialIfRequired(pixel, fragProperties))
				return false;

			if (!ShaderPropertyDefinition::VerifySimilarity(vertexProperties, fragProperties))
			{
				LINA_ERR("LinaMaterial structs in vertex and fragment shaders are different!");
				return false;
			}
			m_meta.propertyDefinitions = fragProperties.empty() ? vertexProperties : fragProperties;
			return true;
		};

		if (!injectMaterials(vertexBlock, fragBlock))
			return false;
		m_meta.shaderType = ShaderPreprocessor::GetShaderType(txt);

		ClearVariantCompileData();
		m_meta.variants.clear();
		ShaderPreprocessor::ExtractVariants(txt, m_meta.variants);

		if (s_variantInjection)
			s_variantInjection(this);

		for (ShaderVariant& variant : m_meta.variants)
		{
			variant.id = TO_SID(variant.name);

			String vertexShader = "", fragShader = "";

			if (variant.vertexWrap.empty())
			{
				vertexShader = vertexBlock;
			}
			else
			{
				if (!FileSystem::FileOrPathExists(variant.vertexWrap))
				{
					LINA_ERR("Variant vertex wrapped does not exist! {0}", variant.vertexWrap);
					return false;
				}

				const String vertexWrap = FileSystem::ReadFileContentsAsString(variant.vertexWrap);
				String		 outWrap = "", outDummyWrap = "";
				ShaderPreprocessor::ExtractVertexFrag(vertexWrap, outWrap, outDummyWrap);
				const String rpInclude0 = "#include \"Resources/Core/Shaders/Common/GlobalData.linashader\"\n";
				const String rpInclude1 = "#include \"" + variant.renderPass + "\"\n";
				outWrap.insert(0, rpInclude0);
				outWrap.insert(rpInclude0.length(), rpInclude1);
				ShaderPreprocessor::InjectUserShader(outWrap, vertexBlock);

				vertexShader = outWrap;
			}

			if (variant.fragWrap.empty())
			{
				fragShader = fragBlock;
			}
			else
			{
				if (!FileSystem::FileOrPathExists(variant.fragWrap))
				{
					LINA_ERR("Variant frag wrapped does not exist!");
					return false;
				}

				const String wrap	 = FileSystem::ReadFileContentsAsString(variant.fragWrap);
				String		 outWrap = "", outDummyWrap = "";
				ShaderPreprocessor::ExtractVertexFrag(wrap, outDummyWrap, outWrap);
				const String rpInclude0 = "#include \"Resources/Core/Shaders/Common/GlobalData.linashader\"\n";
				const String rpInclude1 = "#include \"" + variant.renderPass + "\"\n";
				outWrap.insert(0, rpInclude0);
				outWrap.insert(rpInclude0.length(), rpInclude1);
				ShaderPreprocessor::InjectUserShader(outWrap, fragBlock);

				fragShader = outWrap;
			}
			ShaderPreprocessor::InjectVersionAndExtensions(vertexShader, true);
			ShaderPreprocessor::InjectVersionAndExtensions(fragShader, true);

			variant._compileData.push_back({
				.stage		 = LinaGX::ShaderStage::Vertex,
				.text		 = vertexShader,
				.includePath = includePath.c_str(),
			});

			variant._compileData.push_back({
				.stage		 = LinaGX::ShaderStage::Fragment,
				.text		 = fragShader,
				.includePath = includePath.c_str(),
			});
		}

		return CompileVariants();
	}

	void Shader::SaveToStream(OStream& stream) const
	{
		Resource::SaveToStream(stream);
		stream << VERSION;
		stream << m_meta;
	}

	void Shader::LoadFromStream(IStream& stream)
	{
		Resource::LoadFromStream(stream);
		uint32 version = 0;
		stream >> version;
		stream >> m_meta;
	}

	void Shader::GenerateHW()
	{
		LINA_ASSERT(m_hwValid == false, "");

		m_gpuHandles.clear();

		// Create variants
		for (ShaderVariant& variant : m_meta.variants)
		{

			const bool success = LinaGX::Instance::CompileShaderFromSPV(variant._compileData, variant._outLayout);

			if (!success)
			{
				LINA_ERR("Failed compiling shader from SPV!");
				continue;
			}

			LinaGX::ColorBlendAttachment blend = LinaGX::ColorBlendAttachment{
				.blendEnabled		 = !variant.blendDisable,
				.srcColorBlendFactor = variant.blendSrcFactor,
				.dstColorBlendFactor = variant.blendDstFactor,
				.colorBlendOp		 = variant.blendColorOp,
				.srcAlphaBlendFactor = variant.blendSrcAlphaFactor,
				.dstAlphaBlendFactor = variant.blendDstAlphaFactor,
				.alphaBlendOp		 = variant.blendAlphaOp,
				.componentFlags		 = variant.componentFlags,
			};

			LINAGX_VEC<LinaGX::ShaderColorAttachment> colorAttachments;
			colorAttachments.resize(variant.targets.size());

			for (size_t i = 0; i < variant.targets.size(); i++)
			{
				colorAttachments[i] = {
					.format			 = variant.targets[i].format,
					.blendAttachment = blend,
				};
			}

			LinaGX::ShaderDepthStencilDesc depthStencilAtt = {
				.depthStencilAttachmentFormat = variant.depthFormat,
				.depthWrite					  = variant.depthWrite,
				.depthTest					  = variant.depthTest,
				.depthCompare				  = variant.depthOp,
			};

			const uint32 handle = Application::GetLGX()->CreateShader({
				.stages					 = variant._compileData,
				.colorAttachments		 = colorAttachments,
				.depthStencilDesc		 = depthStencilAtt,
				.layout					 = variant._outLayout,
				.polygonMode			 = LinaGX::PolygonMode::Fill,
				.cullMode				 = variant.cullMode,
				.frontFace				 = variant.frontFace,
				.topology				 = variant.topology,
				.samples				 = variant.msaaSamples,
				.enableSampleShading	 = variant.enableSampleShading,
				.depthBiasEnable		 = variant.depthBiasEnable,
				.depthBiasConstant		 = variant.depthBiasConstant,
				.depthBiasClamp			 = variant.depthBiasClamp,
				.depthBiasSlope			 = variant.depthBiasSlope,
				.useCustomPipelineLayout = false,
				.debugName				 = m_name.c_str(),
			});

			m_gpuHandles.push_back({variant.id, handle});
			for (LinaGX::ShaderCompileData& data : variant._compileData)
				delete[] data.outBlob.ptr;

			variant._compileData.clear();
		}

		m_hwValid = true;
	}

	void Shader::DestroyHW()
	{
		LINA_ASSERT(m_hwValid, "");
		m_hwValid		= false;
		m_hwUploadValid = false;

		for (Pair<StringID, uint32> pair : m_gpuHandles)
			Application::GetLGX()->DestroyShader(pair.second);
	}

} // namespace Lina
