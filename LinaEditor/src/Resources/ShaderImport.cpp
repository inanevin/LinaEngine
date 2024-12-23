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

#include "Editor/Resources/ShaderImport.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Core/Graphics/Utility/ShaderPreprocessor.hpp"

namespace Lina::Editor
{
	bool ShaderImport::ImportShader(Shader* shader, const String& rawPath)
	{
		if (!FileSystem::FileOrPathExists(rawPath))
			return false;

		const String includePath = FileSystem::GetRunningDirectory();
		if (includePath.empty())
			return false;

		const String rawStr = FileSystem::ReadFileContentsAsString(rawPath);
		if (rawStr.empty())
			return false;

		if (!ShaderPreprocessor::VerifyFullShader(rawStr))
			return false;

		String vertexBlock = "", fragBlock = "";
		if (!ShaderPreprocessor::ExtractVertexFrag(rawStr, vertexBlock, fragBlock))
			return false;

		Shader::Metadata& meta = shader->GetMeta();

		auto injectMaterials = [&meta](String& vertex, String& pixel) -> bool {
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
			meta.propertyDefinitions = fragProperties.empty() ? vertexProperties : fragProperties;
			return true;
		};

		if (!injectMaterials(vertexBlock, fragBlock))
		{
			return false;
		}

		meta.shaderType = ShaderPreprocessor::GetShaderType(rawStr);

		if (meta.shaderType != ShaderType::Custom)
		{
			for (ShaderVariant& variant : meta.variants)
			{
				for (LinaGX::ShaderCompileData& data : variant._compileData)
				{
					if (data.outBlob.ptr != nullptr)
						delete[] data.outBlob.ptr;
				}
				variant._compileData.clear();
			}

			meta.variants.clear();
		}

		if (meta.shaderType == ShaderType::Custom)
		{
			ShaderPreprocessor::InjectVersionAndExtensions(vertexBlock);
			ShaderPreprocessor::InjectVersionAndExtensions(fragBlock);

			for (ShaderVariant& variant : meta.variants)
			{
				variant._compileData.clear();

				variant._compileData.push_back({
					.stage		 = LinaGX::ShaderStage::Vertex,
					.text		 = vertexBlock,
					.includePath = includePath.c_str(),
				});

				variant._compileData.push_back({
					.stage		 = LinaGX::ShaderStage::Fragment,
					.text		 = fragBlock,
					.includePath = includePath.c_str(),
				});
			}
		}
		else if (meta.shaderType == ShaderType::Lighting)
		{
			const String vertex = ShaderPreprocessor::MakeVariantBlock(vertexBlock, "Resources/Core/Shaders/Common/MainVertexSimple.linashader", "Resources/Core/Shaders/Common/RenderPass_Forward.linashader");
			const String frag	= ShaderPreprocessor::MakeVariantBlock(fragBlock, "Resources/Core/Shaders/Common/MainFragSimple.linashader", "Resources/Core/Shaders/Common/RenderPass_Forward.linashader");
			meta.variants.push_back(ShaderPreprocessor::MakeVariant("Default", vertex, frag, LinaGX::CullMode::None, BlendMode::Opaque, DepthTesting::None, RenderPassType::RENDER_PASS_FORWARD));
		}
		else if (meta.shaderType == ShaderType::Sky)
		{
			const String  vertex  = ShaderPreprocessor::MakeVariantBlock(vertexBlock, "Resources/Core/Shaders/Common/MainVertexSky.linashader", "Resources/Core/Shaders/Common/RenderPass_Forward.linashader");
			const String  frag	  = ShaderPreprocessor::MakeVariantBlock(fragBlock, "Resources/Core/Shaders/Common/MainFragSimple.linashader", "Resources/Core/Shaders/Common/RenderPass_Forward.linashader");
			ShaderVariant variant = ShaderPreprocessor::MakeVariant("Default", vertex, frag, LinaGX::CullMode::None, BlendMode::Opaque, DepthTesting::Test, RenderPassType::RENDER_PASS_FORWARD);
			variant.depthOp		  = LinaGX::CompareOp::Equal;
			meta.variants.push_back(variant);
		}
		else if (meta.shaderType == ShaderType::DeferredSurface)
		{
			// Static
			{
				const String vertex = ShaderPreprocessor::MakeVariantBlock(vertexBlock, "Resources/Core/Shaders/Common/MainVertexStatic.linashader", "Resources/Core/Shaders/Common/RenderPass_Deferred.linashader");
				const String frag	= ShaderPreprocessor::MakeVariantBlock(fragBlock, "Resources/Core/Shaders/Common/MainFragSurface.linashader", "Resources/Core/Shaders/Common/RenderPass_Deferred.linashader");
				meta.variants.push_back(ShaderPreprocessor::MakeVariant("Static", vertex, frag, LinaGX::CullMode::Back, BlendMode::Opaque, DepthTesting::TestWrite, RenderPassType::RENDER_PASS_DEFERRED));
			}

			// Skinned
			{
				const String vertex = ShaderPreprocessor::MakeVariantBlock(vertexBlock, "Resources/Core/Shaders/Common/MainVertexSkinned.linashader", "Resources/Core/Shaders/Common/RenderPass_Deferred.linashader");
				const String frag	= ShaderPreprocessor::MakeVariantBlock(fragBlock, "Resources/Core/Shaders/Common/MainFragSurface.linashader", "Resources/Core/Shaders/Common/RenderPass_Deferred.linashader");
				meta.variants.push_back(ShaderPreprocessor::MakeVariant("Skinned", vertex, frag, LinaGX::CullMode::Back, BlendMode::Opaque, DepthTesting::TestWrite, RenderPassType::RENDER_PASS_DEFERRED));
			}

			// Static - Entity ID Support
			{
				const String vertex = ShaderPreprocessor::MakeVariantBlock(vertexBlock, "Resources/Editor/Shaders/Common/MainVertexStatic_EntityID.linashader", "Resources/Editor/Shaders/Common/RenderPass_EntityID.linashader");
				const String frag	= ShaderPreprocessor::MakeVariantBlock(fragBlock, "Resources/Editor/Shaders/Common/MainFrag_EntityID.linashader", "Resources/Editor/Shaders/Common/RenderPass_EntityID.linashader");
				meta.variants.push_back(ShaderPreprocessor::MakeVariant("StaticEntityID", vertex, frag, LinaGX::CullMode::Back, BlendMode::Opaque, DepthTesting::TestWrite, RenderPassType::RENDER_PASS_FORWARD));
			}

			// Skinned - Entity ID Support
			{
				const String vertex = ShaderPreprocessor::MakeVariantBlock(vertexBlock, "Resources/Editor/Shaders/Common/MainVertexSkinned_EntityID.linashader", "Resources/Editor/Shaders/Common/RenderPass_EntityID.linashader");
				const String frag	= ShaderPreprocessor::MakeVariantBlock(fragBlock, "Resources/Editor/Shaders/Common/MainFrag_EntityID.linashader", "Resources/Editor/Shaders/Common/RenderPass_EntityID.linashader");
				meta.variants.push_back(ShaderPreprocessor::MakeVariant("SkinnedEntityID", vertex, frag, LinaGX::CullMode::Back, BlendMode::Opaque, DepthTesting::TestWrite, RenderPassType::RENDER_PASS_FORWARD));
			}
		}
		else if (meta.shaderType == ShaderType::ForwardSurface)
		{
			// Static
			{
				const String vertex = ShaderPreprocessor::MakeVariantBlock(vertexBlock, "Resources/Core/Shaders/Common/MainVertexStatic.linashader", "Resources/Core/Shaders/Common/RenderPass_Forward.linashader");
				const String frag	= ShaderPreprocessor::MakeVariantBlock(fragBlock, "Resources/Core/Shaders/Common/MainFragSimple.linashader", "Resources/Core/Shaders/Common/RenderPass_Forward.linashader");
				meta.variants.push_back(ShaderPreprocessor::MakeVariant("Static", vertex, frag, LinaGX::CullMode::Back, BlendMode::AlphaBlend, DepthTesting::TestWrite, RenderPassType::RENDER_PASS_FORWARD));
			}

			// Skinned
			{
				const String vertex = ShaderPreprocessor::MakeVariantBlock(vertexBlock, "Resources/Core/Shaders/Common/MainVertexSkinned.linashader", "Resources/Core/Shaders/Common/RenderPass_Forward.linashader");
				const String frag	= ShaderPreprocessor::MakeVariantBlock(fragBlock, "Resources/Core/Shaders/Common/MainFragSimple.linashader", "Resources/Core/Shaders/Common/RenderPass_Forward.linashader");
				meta.variants.push_back(ShaderPreprocessor::MakeVariant("Skinned", vertex, frag, LinaGX::CullMode::Back, BlendMode::AlphaBlend, DepthTesting::TestWrite, RenderPassType::RENDER_PASS_FORWARD));
			}

			// Static - Entity ID Support
			{
				const String vertex = ShaderPreprocessor::MakeVariantBlock(vertexBlock, "Resources/Editor/Shaders/Common/MainVertexStatic_EntityID.linashader", "Resources/Editor/Shaders/Common/RenderPass_EntityID.linashader");
				const String frag	= ShaderPreprocessor::MakeVariantBlock(fragBlock, "Resources/Editor/Shaders/Common/MainFrag_EntityID.linashader", "Resources/Core/Shaders/Common/RenderPass_EntityID.linashader");
				meta.variants.push_back(ShaderPreprocessor::MakeVariant("StaticEntityID", vertex, frag, LinaGX::CullMode::Back, BlendMode::Opaque, DepthTesting::TestWrite, RenderPassType::RENDER_PASS_FORWARD));
			}

			// Skinned - Entity ID Support
			{
				const String vertex = ShaderPreprocessor::MakeVariantBlock(vertexBlock, "Resources/Editor/Shaders/Common/MainVertexSkinned_EntityID.linashader", "Resources/Editor/Shaders/Common/RenderPass_EntityID.linashader");
				const String frag	= ShaderPreprocessor::MakeVariantBlock(fragBlock, "Resources/Editor/Shaders/Common/MainFrag_EntityID.linashader", "Resources/Core/Shaders/Common/RenderPass_EntityID.linashader");
				meta.variants.push_back(ShaderPreprocessor::MakeVariant("SkinnedEntityID", vertex, frag, LinaGX::CullMode::Back, BlendMode::Opaque, DepthTesting::TestWrite, RenderPassType::RENDER_PASS_FORWARD));
			}
		}

		return shader->CompileVariants();
	}

} // namespace Lina::Editor
