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
#include "Common/System/SystemInfo.hpp"
#include "Core/Graphics/Utility/ShaderPreprocessor.hpp"

namespace Lina::Editor
{
	void ShaderImport::InjectShaderVariant(Shader* shader)
	{
		if (shader->GetShaderType() == ShaderType::DeferredSurface || shader->GetShaderType() == ShaderType::ForwardSurface)
		{
			ShaderVariant outlineStatic	 = {};
			outlineStatic.vertexWrap	 = "Resources/Core/Shaders/Common/MainVertexStatic.linashader";
			outlineStatic.fragWrap		 = "Resources/Editor/Shaders/Common/MainFragOutline.linashader";
			outlineStatic.name			 = "StaticOutline";
			outlineStatic.renderPassName = "Outline";
			outlineStatic.renderPass	 = "Resources/Editor/Shaders/Common/RenderPass_World.linashader";
			outlineStatic.targets		 = {{
					   .format = SystemInfo::GetLDRFormat(),
			   }};
            outlineStatic.depthFormat = SystemInfo::GetDepthFormat();
			ShaderPreprocessor::ApplyBlending(outlineStatic, BlendMode::Opaque);
			ShaderPreprocessor::ApplyDepth(outlineStatic, DepthTesting::Less);
			shader->GetMeta().variants.push_back(outlineStatic);

			ShaderVariant outlineSkinned  = {};
			outlineSkinned.vertexWrap	  = "Resources/Core/Shaders/Common/MainVertexSkinned.linashader";
			outlineSkinned.fragWrap		  = "Resources/Editor/Shaders/Common/MainFragOutline.linashader";
			outlineSkinned.name			  = "SkinnedOutline";
			outlineSkinned.renderPassName = "Outline";
			outlineSkinned.renderPass	  = "Resources/Editor/Shaders/Common/RenderPass_World.linashader";
			outlineSkinned.targets		  = {{
					   .format = SystemInfo::GetLDRFormat(),
			   }};
            outlineSkinned.depthFormat = SystemInfo::GetDepthFormat();
			ShaderPreprocessor::ApplyBlending(outlineSkinned, BlendMode::Opaque);
			ShaderPreprocessor::ApplyDepth(outlineSkinned, DepthTesting::Less);
			shader->GetMeta().variants.push_back(outlineSkinned);

			ShaderVariant entityIDStatic  = {};
			entityIDStatic.vertexWrap	  = "Resources/Editor/Shaders/Common/MainVertexStaticEntityID.linashader";
			entityIDStatic.fragWrap		  = "Resources/Editor/Shaders/Common/MainFragEntityID.linashader";
			entityIDStatic.renderPass	  = "Resources/Editor/Shaders/Common/RenderPass_World.linashader";
			entityIDStatic.name			  = "StaticEntityID";
			entityIDStatic.renderPassName = "EntityID";
			entityIDStatic.targets		  = {{LinaGX::Format::R32_UINT}};
            entityIDStatic.depthFormat = SystemInfo::GetDepthFormat();

			ShaderPreprocessor::ApplyBlending(entityIDStatic, BlendMode::Opaque);
			ShaderPreprocessor::ApplyDepth(entityIDStatic, DepthTesting::Less);
			shader->GetMeta().variants.push_back(entityIDStatic);

			ShaderVariant entityIDSkinned  = {};
			entityIDSkinned.vertexWrap	   = "Resources/Editor/Shaders/Common/MainVertexSkinnedEntityID.linashader";
			entityIDSkinned.fragWrap	   = "Resources/Editor/Shaders/Common/MainFragEntityID.linashader";
			entityIDSkinned.targets		   = {{LinaGX::Format::R32_UINT}};
			entityIDSkinned.renderPass	   = "Resources/Editor/Shaders/Common/RenderPass_World.linashader";
			entityIDSkinned.name		   = "SkinnedEntityID";
			entityIDSkinned.renderPassName = "EntityID";
            entityIDSkinned.depthFormat = SystemInfo::GetDepthFormat();

			ShaderPreprocessor::ApplyBlending(entityIDSkinned, BlendMode::Opaque);
			ShaderPreprocessor::ApplyDepth(entityIDSkinned, DepthTesting::Less);
			shader->GetMeta().variants.push_back(entityIDSkinned);
		}
	}

} // namespace Lina::Editor
