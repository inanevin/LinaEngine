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

#include "Editor/EditorApplicationDelegate.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Widgets/Testbed.hpp"
#include "Core/Application.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Renderers/SurfaceRenderer.hpp"
#include "Core/GUI/Theme.hpp"

namespace Lina
{
	SystemInitializationInfo Lina_GetInitInfo()
	{
		return SystemInitializationInfo{
			.appName			 = "Lina Editor",
			.windowWidth		 = 800,
			.windowHeight		 = 600,
			.windowStyle		 = LinaGX::WindowStyle::BorderlessApplication,
			.appListener		 = new Lina::Editor::EditorApplicationDelegate(),
			.resourceManagerMode = Lina::ResourceManagerMode::File,
		};
	}
} // namespace Lina

namespace Lina::Editor
{
	void EditorApplicationDelegate::OnPlatformSetup()
	{
		ThemeDef theme		  = {};
		theme.defaultFont	  = DEFAULT_FONT_SID;
		theme.alternativeFont = DEFAULT_FONT_SID;
		Theme::SetDef(theme);
	}
	void EditorApplicationDelegate::RegisterAppResources(ResourceManager& rm)
	{
		ApplicationDelegate::RegisterAppResources(rm);

		Vector<ResourceIdentifier> resources;

		// Priority
		resources.push_back(ResourceIdentifier(DEFAULT_FONT_PATH, GetTypeID<Font>(), 0, true, ResourceTag::Priority));

		// Core
		resources.push_back(ResourceIdentifier(ICON_FONT_PATH, GetTypeID<Font>(), 0, true, ResourceTag::Core));

		for (auto& r : resources)
			r.sid = TO_SID(r.path);

		rm.RegisterAppResources(resources);
	}

	bool EditorApplicationDelegate::FillResourceCustomMeta(StringID sid, OStream& stream)
	{
		if (ApplicationDelegate::FillResourceCustomMeta(sid, stream))
			return true;

		if (sid == DEFAULT_FONT_SID)
		{
			Font::Metadata customMeta = {
				.points		 = {{.size = 12, .dpiLimit = 1.1f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 16, .dpiLimit = 10.0f}},
				.isSDF		 = false,
				.glyphRanges = {linatl::make_pair(160, 360)},
			};
			customMeta.SaveToStream(stream);
			return true;
		}

		// NOTE: 160, 380 is the glyph range for nunito sans

		if (sid == ICON_FONT_SID)
		{
			Font::Metadata customMeta = {
				.points = {{.size = 32, .dpiLimit = 10.0f}},
				.isSDF	= true,
			};
			customMeta.SaveToStream(stream);
			return true;
		}

		return false;
	}

	void EditorApplicationDelegate::OnPreInitialize()
	{
	}

	void EditorApplicationDelegate::OnInitialize()
	{
		auto*	 gfxManager		 = m_app->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		auto&	 widgetAllocator = gfxManager->GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN)->GetWidgetManager();
		Testbed* tb				 = widgetAllocator.GetRoot()->Allocate<Testbed>();
		widgetAllocator.GetRoot()->AddChild(tb);
	}

} // namespace Lina::Editor
