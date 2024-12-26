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

#include "Editor/Resources/EditorResources.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Resources/ShaderImport.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/GfxContext.hpp"

namespace Lina::Editor
{

	bool EditorResources::LoadPriorityResources(ResourceManagerV2& manager)
	{
		Shader::Metadata meta = {};

		meta.variants.push_back({});
		ShaderVariant& variant = meta.variants.back();

		variant = ShaderVariant{
			.id					 = "Swapchain"_hs,
			.blendDisable		 = false,
			.blendSrcFactor		 = LinaGX::BlendFactor::SrcAlpha,
			.blendDstFactor		 = LinaGX::BlendFactor::OneMinusSrcAlpha,
			.blendColorOp		 = LinaGX::BlendOp::Add,
			.blendSrcAlphaFactor = LinaGX::BlendFactor::One,
			.blendDstAlphaFactor = LinaGX::BlendFactor::Zero,
			.blendAlphaOp		 = LinaGX::BlendOp::Add,
			.depthTest			 = false,
			.depthWrite			 = false,
			.depthFormat		 = LinaGX::Format::UNDEFINED,
			.targets			 = {{.format = DEFAULT_SWAPCHAIN_FORMAT}},
			.cullMode			 = LinaGX::CullMode::None,
			.frontFace			 = LinaGX::FrontFace::CCW,
		};

		Font::Metadata fontMeta1 = {
			.points = {{.size = 32, .dpiLimit = 10.0f}},
			.isSDF	= true,
		};

		Font::Metadata fontMeta2 = {
			.points = {{.size = 14, .dpiLimit = 1.0f}, {.size = 18, .dpiLimit = 1.8f}, {.size = 22, .dpiLimit = 10.0f}},
			.isSDF	= false,
		};

		Font::Metadata fontMeta3 = {
			.points = {{.size = 20, .dpiLimit = 1.0f}, {.size = 24, .dpiLimit = 1.8f}, {.size = 28, .dpiLimit = 10.0f}},
			.isSDF	= false,
		};

		OStream shaderStream;
		meta.SaveToStream(shaderStream);

		OStream fontStream1, fontStream2, fontStream3;
		fontMeta1.SaveToStream(fontStream1);
		fontMeta2.SaveToStream(fontStream2);
		fontMeta3.SaveToStream(fontStream3);

		ResourceDefinitionList defs = {
			{
				.id			= EDITOR_SHADER_GUI_SDF_TEXT_ID,
				.name		= EDITOR_SHADER_GUI_SDF_TEXT_PATH,
				.tid		= GetTypeID<Shader>(),
				.customMeta = shaderStream,
			},
			{
				.id			= EDITOR_SHADER_GUI_TEXT_ID,
				.name		= EDITOR_SHADER_GUI_TEXT_PATH,
				.tid		= GetTypeID<Shader>(),
				.customMeta = shaderStream,
			},
			{
				.id			= EDITOR_SHADER_GUI_HUE_DISPLAY_ID,
				.name		= EDITOR_SHADER_GUI_HUE_DISPLAY_PATH,
				.tid		= GetTypeID<Shader>(),
				.customMeta = shaderStream,
			},
			{
				.id			= EDITOR_SHADER_GUI_COLOR_WHEEL_ID,
				.name		= EDITOR_SHADER_GUI_COLOR_WHEEL_PATH,
				.tid		= GetTypeID<Shader>(),
				.customMeta = shaderStream,
			},
			{
				.id			= EDITOR_SHADER_GUI_DEFAULT_ID,
				.name		= EDITOR_SHADER_GUI_DEFAULT_PATH,
				.tid		= GetTypeID<Shader>(),
				.customMeta = shaderStream,
			},
			{
				.id			= EDITOR_SHADER_GUI_GLITCH_ID,
				.name		= EDITOR_SHADER_GUI_GLITCH_PATH,
				.tid		= GetTypeID<Shader>(),
				.customMeta = shaderStream,
			},
			{
				.id	  = EDITOR_TEXTURE_LINA_LOGO_ID,
				.name = EDITOR_TEXTURE_LINA_LOGO_PATH,
				.tid  = GetTypeID<Texture>(),
			},
			{
				.id	  = EDITOR_TEXTURE_LINA_LOGO_BOTTOM_ID,
				.name = EDITOR_TEXTURE_LINA_LOGO_BOTTOM_PATH,
				.tid  = GetTypeID<Texture>(),
			},
			{
				.id	  = EDITOR_TEXTURE_LINA_LOGO_LEFT_ID,
				.name = EDITOR_TEXTURE_LINA_LOGO_LEFT_PATH,
				.tid  = GetTypeID<Texture>(),
			},
			{
				.id	  = EDITOR_TEXTURE_LINA_LOGO_RIGHT_ID,
				.name = EDITOR_TEXTURE_LINA_LOGO_RIGHT_PATH,
				.tid  = GetTypeID<Texture>(),
			},
			{
				.id			= EDITOR_FONT_ICON_ID,
				.name		= EDITOR_FONT_ICON_PATH,
				.tid		= GetTypeID<Font>(),
				.customMeta = fontStream1,
			},
			{
				.id			= EDITOR_FONT_ROBOTO_ID,
				.name		= EDITOR_FONT_ROBOTO_PATH,
				.tid		= GetTypeID<Font>(),
				.customMeta = fontStream2,
			},
			{
				.id			= EDITOR_FONT_ROBOTO_BOLD_ID,
				.name		= EDITOR_FONT_ROBOTO_BOLD_PATH,
				.tid		= GetTypeID<Font>(),
				.customMeta = fontStream2,
			},
			{
				.id			= EDITOR_FONT_PLAY_BIG_ID,
				.name		= EDITOR_FONT_PLAY_BIG_PATH,
				.tid		= GetTypeID<Font>(),
				.customMeta = fontStream3,
			},
		};

		manager.LoadResourcesFromFile(defs, NULL);

		fontStream1.Destroy();
		fontStream2.Destroy();
		fontStream3.Destroy();
		shaderStream.Destroy();

		for (const ResourceDef& def : defs)
		{
			Resource* res = manager.GetIfExists(def.tid, def.id);

			if (res == nullptr)
				return false;
		}

		return true;
	}

	void EditorResources::StartLoadCoreResources(ResourceManagerV2& manager)
	{
		Texture* txtCheckered			= manager.CreateResource<Texture>(EDITOR_TEXTURE_CHECKERED_ID, EDITOR_TEXTURE_CHECKERED_PATH);
		Texture* txtProtoDark			= manager.CreateResource<Texture>(EDITOR_TEXTURE_PROTOTYPE_DARK_ID, EDITOR_TEXTURE_PROTOTYPE_DARK_PATH);
		Font*	 fontPlay				= manager.CreateResource<Font>(EDITOR_FONT_PLAY_ID, EDITOR_FONT_PLAY_PATH);
		Font*	 fontPlayBold			= manager.CreateResource<Font>(EDITOR_FONT_PLAY_BOLD_ID, EDITOR_FONT_PLAY_BOLD_PATH);
		Font*	 fontPlayVeryBig		= manager.CreateResource<Font>(EDITOR_FONT_PLAY_VERY_BIG_ID, EDITOR_FONT_PLAY_VERY_BIG_PATH);
		Shader*	 shaderWorldGrid		= manager.CreateResource<Shader>(EDITOR_SHADER_WORLD_GRID_ID, EDITOR_SHADER_WORLD_GRID_PATH);
		Shader*	 shaderWorldEntityID	= manager.CreateResource<Shader>(EDITOR_SHADER_WORLD_ENTITYID_ID, EDITOR_SHADER_WORLD_ENTITYID_PATH);
		Model*	 gizmoTranslate			= manager.CreateResource<Model>(EDITOR_MODEL_GIZMO_TRANSLATE_ID, EDITOR_MODEL_GIZMO_TRANSLATE_PATH);
		Model*	 gizmoRotate			= manager.CreateResource<Model>(EDITOR_MODEL_GIZMO_ROTATE_ID, EDITOR_MODEL_GIZMO_ROTATE_PATH);
		Model*	 gizmoScale				= manager.CreateResource<Model>(EDITOR_MODEL_GIZMO_SCALE_ID, EDITOR_MODEL_GIZMO_SCALE_PATH);
		Shader*	 gizmo					= manager.CreateResource<Shader>(EDITOR_SHADER_WORLD_GIZMO_ID, EDITOR_SHADER_WORLD_GIZMO_PATH);
		Shader*	 outlineFullscreen		= manager.CreateResource<Shader>(EDITOR_SHADER_WORLD_OUTLINE_FULLSCREEN_ID, EDITOR_SHADER_WORLD_OUTLINE_FULLSCREEN_PATH);
		Shader*	 line3D					= manager.CreateResource<Shader>(EDITOR_SHADER_WORLD_LINE3D_ID, EDITOR_SHADER_WORLD_LINE3D_PATH);
		Shader*	 shaderLinaVG3D			= manager.CreateResource<Shader>(EDITOR_SHADER_WORLD_LVG3D_ID, EDITOR_SHADER_WORLD_LVG3D_PATH);
		Shader*	 shaderWorldSample		= manager.CreateResource<Shader>(EDITOR_SHADER_WORLD_SAMPLE_ID, EDITOR_SHADER_WORLD_SAMPLE_PATH);
		Shader*	 shaderWorldDepthSample = manager.CreateResource<Shader>(EDITOR_SHADER_WORLD_DEPTH_SAMPLE_ID, EDITOR_SHADER_WORLD_DEPTH_SAMPLE_PATH);

		m_createdResources.insert(txtCheckered);
		m_createdResources.insert(txtProtoDark);
		m_createdResources.insert(fontPlay);
		m_createdResources.insert(fontPlayBold);
		m_createdResources.insert(fontPlayVeryBig);
		m_createdResources.insert(shaderWorldGrid);
		m_createdResources.insert(shaderWorldEntityID);
		m_createdResources.insert(gizmoTranslate);
		m_createdResources.insert(gizmoRotate);
		m_createdResources.insert(gizmoScale);
		m_createdResources.insert(gizmo);
		m_createdResources.insert(outlineFullscreen);
		m_createdResources.insert(line3D);
		m_createdResources.insert(shaderLinaVG3D);
		m_createdResources.insert(shaderWorldSample);
		m_createdResources.insert(shaderWorldDepthSample);

		fontPlay->GetMeta() = {
			.points = {{.size = 14, .dpiLimit = 1.1f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 16, .dpiLimit = 10.0f}},
			.isSDF	= false,
		};

		fontPlayBold->GetMeta() = {
			.points = {{.size = 14, .dpiLimit = 1.1f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 16, .dpiLimit = 10.0f}},
			.isSDF	= false,
		};

		fontPlayVeryBig->GetMeta() = {
			.points = {{.size = 32, .dpiLimit = 1.0f}, {.size = 36, .dpiLimit = 1.8f}, {.size = 38, .dpiLimit = 10.0f}},
			.isSDF	= false,
		};

		shaderWorldGrid->GetMeta().variants.push_back({
			ShaderVariant{
				.id					 = "Default"_hs,
				.name				 = "Default",
				.blendDisable		 = false,
				.blendSrcFactor		 = LinaGX::BlendFactor::SrcAlpha,
				.blendDstFactor		 = LinaGX::BlendFactor::OneMinusSrcAlpha,
				.blendColorOp		 = LinaGX::BlendOp::Add,
				.blendSrcAlphaFactor = LinaGX::BlendFactor::One,
				.blendDstAlphaFactor = LinaGX::BlendFactor::OneMinusSrcAlpha,
				.blendAlphaOp		 = LinaGX::BlendOp::Add,
				.depthTest			 = true,
				.depthWrite			 = true,
				.depthFormat		 = LinaGX::Format::D32_SFLOAT,
				.targets			 = {{LinaGX::Format::R8G8B8A8_SRGB}},
				.depthOp			 = LinaGX::CompareOp::Less,
				.cullMode			 = LinaGX::CullMode::None,
				.frontFace			 = LinaGX::FrontFace::CCW,
				.topology			 = LinaGX::Topology::TriangleList,
				.msaaSamples		 = EDITOR_WORLD_MSAA_SAMPLES,
			},
		} // namespace Lina::Editor
		);

		gizmo->GetMeta().variants.push_back({
			ShaderVariant{
				.id			  = "Default"_hs,
				.name		  = "Default",
				.blendDisable = true,
				.depthTest	  = false,
				.depthWrite	  = false,
				.depthFormat  = LinaGX::Format::D32_SFLOAT,
				.targets	  = {{LinaGX::Format::R8G8B8A8_SRGB}},
				.cullMode	  = LinaGX::CullMode::None,
				.frontFace	  = LinaGX::FrontFace::CCW,
				.topology	  = LinaGX::Topology::TriangleList,
				.msaaSamples  = EDITOR_WORLD_MSAA_SAMPLES,
			},
		});

		line3D->GetMeta().variants.push_back({
			ShaderVariant{
				.id			  = "Default"_hs,
				.name		  = "Default",
				.blendDisable = true,
				.depthTest	  = false,
				.depthWrite	  = false,
				.depthFormat  = LinaGX::Format::D32_SFLOAT,
				.targets	  = {{LinaGX::Format::R8G8B8A8_SRGB}},
				.cullMode	  = LinaGX::CullMode::None,
				.frontFace	  = LinaGX::FrontFace::CW,
				.topology	  = LinaGX::Topology::TriangleList,
				.msaaSamples  = EDITOR_WORLD_MSAA_SAMPLES,
			},
		});

		shaderWorldEntityID->GetMeta().variants.push_back({
			ShaderVariant{
				.id				   = "Default"_hs,
				.name			   = "Default",
				.blendDisable	   = true,
				.depthTest		   = true,
				.depthWrite		   = true,
				.depthFormat	   = LinaGX::Format::D32_SFLOAT,
				.targets		   = {{LinaGX::Format::R32_UINT}},
				.depthOp		   = LinaGX::CompareOp::Less,
				.cullMode		   = LinaGX::CullMode::Back,
				.frontFace		   = LinaGX::FrontFace::CCW,
				.topology		   = LinaGX::Topology::TriangleList,
				.depthBiasEnable   = false,
				.depthBiasConstant = 0.0f,
				.depthBiasClamp	   = 0.0f,
				.depthBiasSlope	   = 0.0f,
			},
		});

		shaderLinaVG3D->GetMeta().variants.push_back({
			ShaderVariant{
				.id					 = "Default"_hs,
				.name				 = "Default",
				.blendDisable		 = false,
				.blendSrcFactor		 = LinaGX::BlendFactor::SrcAlpha,
				.blendDstFactor		 = LinaGX::BlendFactor::OneMinusSrcAlpha,
				.blendColorOp		 = LinaGX::BlendOp::Add,
				.blendSrcAlphaFactor = LinaGX::BlendFactor::One,
				.blendDstAlphaFactor = LinaGX::BlendFactor::OneMinusSrcAlpha,
				.blendAlphaOp		 = LinaGX::BlendOp::Add,
				.depthTest			 = true,
				.depthWrite			 = true,
				.depthFormat		 = LinaGX::Format::D32_SFLOAT,
				.targets			 = {{LinaGX::Format::R8G8B8A8_SRGB}},
				.depthOp			 = LinaGX::CompareOp::Less,
				.cullMode			 = LinaGX::CullMode::None,
				.frontFace			 = LinaGX::FrontFace::CCW,
				.topology			 = LinaGX::Topology::TriangleList,
				.msaaSamples		 = EDITOR_WORLD_MSAA_SAMPLES,
			},
		});

		outlineFullscreen->GetMeta().variants.push_back({
			ShaderVariant{
				.id					 = "Default"_hs,
				.name				 = "Default",
				.blendDisable		 = false,
				.blendSrcFactor		 = LinaGX::BlendFactor::SrcAlpha,
				.blendDstFactor		 = LinaGX::BlendFactor::OneMinusSrcAlpha,
				.blendColorOp		 = LinaGX::BlendOp::Add,
				.blendSrcAlphaFactor = LinaGX::BlendFactor::One,
				.blendDstAlphaFactor = LinaGX::BlendFactor::OneMinusSrcAlpha,
				.blendAlphaOp		 = LinaGX::BlendOp::Add,
				.depthTest			 = false,
				.depthWrite			 = false,
				.depthFormat		 = LinaGX::Format::D32_SFLOAT,
				.targets			 = {{LinaGX::Format::R8G8B8A8_SRGB}},
				.cullMode			 = LinaGX::CullMode::None,
				.frontFace			 = LinaGX::FrontFace::CCW,
				.topology			 = LinaGX::Topology::TriangleList,
				.msaaSamples		 = EDITOR_WORLD_MSAA_SAMPLES,
			},
		} // namespace Lina::Editor
		);

		shaderWorldSample->GetMeta().variants.push_back({
			ShaderVariant{
				.id					 = "Default"_hs,
				.name				 = "Default",
				.blendDisable		 = false,
				.blendSrcFactor		 = LinaGX::BlendFactor::SrcAlpha,
				.blendDstFactor		 = LinaGX::BlendFactor::OneMinusSrcAlpha,
				.blendColorOp		 = LinaGX::BlendOp::Add,
				.blendSrcAlphaFactor = LinaGX::BlendFactor::One,
				.blendDstAlphaFactor = LinaGX::BlendFactor::OneMinusSrcAlpha,
				.blendAlphaOp		 = LinaGX::BlendOp::Add,
				.depthTest			 = false,
				.depthWrite			 = false,
				.depthFormat		 = LinaGX::Format::D32_SFLOAT,
				.targets			 = {{LinaGX::Format::R8G8B8A8_SRGB}},
				.cullMode			 = LinaGX::CullMode::None,
				.frontFace			 = LinaGX::FrontFace::CCW,
				.topology			 = LinaGX::Topology::TriangleList,
				.msaaSamples		 = EDITOR_WORLD_MSAA_SAMPLES,
			},
		} // namespace Lina::Editor
		);
		shaderWorldDepthSample->GetMeta().variants.push_back({
			ShaderVariant{
				.id					 = "Default"_hs,
				.name				 = "Default",
				.blendDisable		 = false,
				.blendSrcFactor		 = LinaGX::BlendFactor::SrcAlpha,
				.blendDstFactor		 = LinaGX::BlendFactor::OneMinusSrcAlpha,
				.blendColorOp		 = LinaGX::BlendOp::Add,
				.blendSrcAlphaFactor = LinaGX::BlendFactor::One,
				.blendDstAlphaFactor = LinaGX::BlendFactor::OneMinusSrcAlpha,
				.blendAlphaOp		 = LinaGX::BlendOp::Add,
				.depthTest			 = true,
				.depthWrite			 = true,
				.depthFormat		 = LinaGX::Format::D32_SFLOAT,
				.targets			 = {},
				.depthOp			 = LinaGX::CompareOp::Always,
				.cullMode			 = LinaGX::CullMode::None,
				.frontFace			 = LinaGX::FrontFace::CCW,
				.topology			 = LinaGX::Topology::TriangleList,
				.msaaSamples		 = EDITOR_WORLD_MSAA_SAMPLES,
			},
		} // namespace Lina::Editor
		);

		for (Resource* r : m_createdResources)
			r->SetPath(r->GetName());
	}

	void EditorResources::LoadCoreResources()
	{
		for (Resource* r : m_createdResources)
		{
			if (r->GetTID() == GetTypeID<Shader>())
			{
				if (ShaderImport::ImportShader(static_cast<Shader*>(r), r->GetPath()))
					m_loadedResources.insert(r);
			}
			else
			{
				if (r->LoadFromFile(r->GetPath()))
					m_loadedResources.insert(r);
			}
		}
	}

	bool EditorResources::EndLoadCoreResources(GfxContext& context)
	{
		for (Resource* r : m_loadedResources)
			r->GenerateHW();

		context.OnResourceManagerGeneratedHW(m_loadedResources);

		m_createdResources.clear();
		m_loadedResources.clear();
		return m_loadedResources.size() == m_createdResources.size();
	}

} // namespace Lina::Editor
