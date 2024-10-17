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

	bool EditorResources::LoadPriorityResources(ResourceManagerV2& resourceManager, ResourceList& outResources)
	{
		auto swapchainShaderMeta = [](Shader* shader) {
			Shader::Metadata& meta		  = shader->GetMeta();
			meta.shaderType				  = ShaderType::Custom;
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
		};

		auto loadShader = [&](ResourceID id, const String& path) -> bool {
			Shader* res = resourceManager.CreateResource<Shader>(id);
			res->SetPath(path);
			res->SetName(FileSystem::GetFilenameOnlyFromPath(path));
			swapchainShaderMeta(res);
			outResources.insert(res);
			return res->LoadFromFile(path);
		};

		auto loadTexture = [&](ResourceID id, const String& path) -> bool {
			Texture* res = resourceManager.CreateResource<Texture>(id);
			res->SetPath(path);
			res->SetName(FileSystem::GetFilenameOnlyFromPath(path));
			outResources.insert(res);
			return res->LoadFromFile(path);
		};

		// Shaders
		{
			if (!loadShader(EDITOR_SHADER_GUI_SDF_TEXT_ID, EDITOR_SHADER_GUI_SDF_TEXT_PATH))
				return false;

			if (!loadShader(EDITOR_SHADER_GUI_TEXT_ID, EDITOR_SHADER_GUI_TEXT_PATH))
				return false;

			if (!loadShader(EDITOR_SHADER_GUI_HUE_DISPLAY_ID, EDITOR_SHADER_GUI_HUE_DISPLAY_PATH))
				return false;

			if (!loadShader(EDITOR_SHADER_GUI_COLOR_WHEEL_ID, EDITOR_SHADER_GUI_COLOR_WHEEL_PATH))
				return false;

			if (!loadShader(EDITOR_SHADER_GUI_DEFAULT_ID, EDITOR_SHADER_GUI_DEFAULT_PATH))
				return false;
		}

		// Textures
		{
			if (!loadTexture(EDITOR_TEXTURE_LINA_LOGO_ID, EDITOR_TEXTURE_LINA_LOGO_PATH))
				return false;

			if (!loadTexture(EDITOR_TEXTURE_LINA_LOGO_BOTTOM_ID, EDITOR_TEXTURE_LINA_LOGO_BOTTOM_PATH))
				return false;

			if (!loadTexture(EDITOR_TEXTURE_LINA_LOGO_LEFT_ID, EDITOR_TEXTURE_LINA_LOGO_LEFT_PATH))
				return false;

			if (!loadTexture(EDITOR_TEXTURE_LINA_LOGO_RIGHT_ID, EDITOR_TEXTURE_LINA_LOGO_RIGHT_PATH))
				return false;
		}

		// Fonts
		{
			{
				Font* font		= resourceManager.CreateResource<Font>(EDITOR_FONT_ICON_ID);
				font->GetMeta() = {
					.points = {{.size = 32, .dpiLimit = 10.0f}},
					.isSDF	= true,
				};

				if (!font->LoadFromFile(EDITOR_FONT_ICON_PATH))
					return false;

				font->SetPath(EDITOR_FONT_ICON_PATH);
				font->SetName(FileSystem::GetFilenameOnlyFromPath(font->GetPath()));
				outResources.insert(font);
			}

			{
				Font* font		= resourceManager.CreateResource<Font>(EDITOR_FONT_ROBOTO_ID);
				font->GetMeta() = {
					.points = {{.size = 14, .dpiLimit = 1.0f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 14, .dpiLimit = 10.0f}},
					.isSDF	= false,
				};

				if (!font->LoadFromFile(EDITOR_FONT_ROBOTO_PATH))
					return false;
				font->SetPath(EDITOR_FONT_ROBOTO_PATH);
				font->SetName(FileSystem::GetFilenameOnlyFromPath(font->GetPath()));
				outResources.insert(font);
			}

			{
				Font* font		= resourceManager.CreateResource<Font>(EDITOR_FONT_ROBOTO_BOLD_ID);
				font->GetMeta() = {
					.points = {{.size = 14, .dpiLimit = 1.0f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 14, .dpiLimit = 10.0f}},
					.isSDF	= false,
				};

				if (!font->LoadFromFile(EDITOR_FONT_ROBOTO_BOLD_PATH))
					return false;
				font->SetPath(EDITOR_FONT_ROBOTO_BOLD_PATH);
				font->SetName(FileSystem::GetFilenameOnlyFromPath(font->GetPath()));
				outResources.insert(font);
			}

			{
				Font* font		= resourceManager.CreateResource<Font>(EDITOR_FONT_PLAY_BIG_ID);
				font->GetMeta() = {
					.points = {{.size = 20, .dpiLimit = 10.1f}, {.size = 20, .dpiLimit = 1.8f}, {.size = 20, .dpiLimit = 10.0f}},
					.isSDF	= false,
				};

				if (!font->LoadFromFile(EDITOR_FONT_PLAY_BIG_PATH))
					return false;
				font->SetPath(EDITOR_FONT_PLAY_BIG_PATH);
				font->SetName(FileSystem::GetFilenameOnlyFromPath(font->GetPath()));
				outResources.insert(font);
			}
		}

		return true;
	}

	bool EditorResources::LoadCoreResources(ResourceManagerV2& resourceManager, ResourceList& outResources)
	{
		auto loadTexture = [&](ResourceID id, const String& path) -> bool {
			Texture* res = resourceManager.CreateResource<Texture>(id);
			res->SetPath(path);
			res->SetName(FileSystem::GetFilenameOnlyFromPath(path));
			outResources.insert(res);
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
				Font* font		= resourceManager.CreateResource<Font>(EDITOR_FONT_PLAY_ID);
				font->GetMeta() = {
					.points = {{.size = 14, .dpiLimit = 1.1f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 16, .dpiLimit = 10.0f}},
					.isSDF	= false,
				};

				if (!font->LoadFromFile(EDITOR_FONT_PLAY_PATH))
					return false;
				font->SetPath(EDITOR_FONT_PLAY_PATH);
				font->SetName(FileSystem::GetFilenameOnlyFromPath(font->GetPath()));
				outResources.insert(font);
			}

			{
				Font* font		= resourceManager.CreateResource<Font>(EDITOR_FONT_PLAY_BOLD_ID);
				font->GetMeta() = {
					.points = {{.size = 14, .dpiLimit = 1.1f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 16, .dpiLimit = 10.0f}},
					.isSDF	= false,
				};

				if (!font->LoadFromFile(EDITOR_FONT_PLAY_BOLD_PATH))
					return false;
				font->SetPath(EDITOR_FONT_PLAY_BOLD_PATH);
				font->SetName(FileSystem::GetFilenameOnlyFromPath(font->GetPath()));
				outResources.insert(font);
			}
		}
		return true;
	}

} // namespace Lina::Editor
