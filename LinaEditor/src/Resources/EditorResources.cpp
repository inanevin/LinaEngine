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
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Common/FileSystem/FileSystem.hpp"

namespace Lina::Editor
{

	bool EditorResources::LoadPriorityResources(ResourceManagerV2& manager)
	{
		Shader::Metadata meta		  = {};
		meta.variants["Swapchain"_hs] = ShaderVariant{
			.blendDisable	 = false,
			.depthTest		 = false,
			.depthWrite		 = false,
			.depthFormat	 = LinaGX::Format::UNDEFINED,
			.targets		 = {{.format = DEFAULT_SWAPCHAIN_FORMAT}},
			.cullMode		 = LinaGX::CullMode::None,
			.frontFace		 = LinaGX::FrontFace::CCW,
			.indirectEnabled = true,
		};

		Font::Metadata fontMeta1 = {
			.points = {{.size = 32, .dpiLimit = 10.0f}},
			.isSDF	= true,
		};

		Font::Metadata fontMeta2 = {
			.points = {{.size = 14, .dpiLimit = 1.0f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 14, .dpiLimit = 10.0f}},
			.isSDF	= false,
		};

		Font::Metadata fontMeta3 = {
			.points = {{.size = 20, .dpiLimit = 10.1f}, {.size = 20, .dpiLimit = 1.8f}, {.size = 20, .dpiLimit = 10.0f}},
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

	bool EditorResources::LoadCoreResources()
	{
		auto loadTexture = [&](ResourceID id, const String& path) -> bool {
			Texture* res = new Texture(id, "");
			m_loadedResources.push_back(res);
			res->SetPath(path);
			res->SetName(FileSystem::GetFilenameOnlyFromPath(path));
			return res->LoadFromFile(path);
		};

		// Textures
		{
			if (!loadTexture(EDITOR_TEXTURE_CHECKERED_ID, EDITOR_TEXTURE_CHECKERED_PATH))
				return false;

			if (!loadTexture(EDITOR_TEXTURE_PROTOTYPE_DARK_ID, EDITOR_TEXTURE_PROTOTYPE_DARK_PATH))
				return false;
		}

		// Fonts
		{
			{
				Font* font = new Font(EDITOR_FONT_PLAY_ID, "");
				m_loadedResources.push_back(font);
				font->GetMeta() = {
					.points = {{.size = 14, .dpiLimit = 1.1f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 16, .dpiLimit = 10.0f}},
					.isSDF	= false,
				};

				if (!font->LoadFromFile(EDITOR_FONT_PLAY_PATH))
					return false;
				font->SetPath(EDITOR_FONT_PLAY_PATH);
				font->SetName(FileSystem::GetFilenameOnlyFromPath(font->GetPath()));
			}

			{
				Font* font = new Font(EDITOR_FONT_PLAY_BOLD_ID, "");
				m_loadedResources.push_back(font);
				font->GetMeta() = {
					.points = {{.size = 14, .dpiLimit = 1.1f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 16, .dpiLimit = 10.0f}},
					.isSDF	= false,
				};

				if (!font->LoadFromFile(EDITOR_FONT_PLAY_BOLD_PATH))
					return false;
				font->SetPath(EDITOR_FONT_PLAY_BOLD_PATH);
				font->SetName(FileSystem::GetFilenameOnlyFromPath(font->GetPath()));
			}
		}
		return true;
	}

	void EditorResources::TransferResourcesToManager(ResourceManagerV2& manager)
	{
		for (Resource* res : m_loadedResources)
		{
			Resource* created = manager.CreateResource(res->GetID(), res->GetTID());

			OStream ostream;
			res->SaveToStream(ostream);

			IStream istream;
			istream.Create(ostream.GetDataRaw(), ostream.GetCurrentSize());

			created->LoadFromStream(istream);

			istream.Destroy();
			ostream.Destroy();
		}

		ClearLoadedResources();
	}

	void EditorResources::ClearLoadedResources()
	{
		for (Resource* res : m_loadedResources)
			delete res;

		m_loadedResources.clear();
	}

} // namespace Lina::Editor
