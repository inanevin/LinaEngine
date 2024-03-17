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
#include "Core/Application.hpp"
#include "Core/GUI/Theme.hpp"
#include "Core/CommonCore.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/GUI/CommonGUI.hpp"

#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Checkbox.hpp"
#include "Core/GUI/Widgets/Primitives/ColorField.hpp"
#include "Core/GUI/Widgets/Primitives/ColorWheel.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Slider.hpp"
#include "Core/GUI/Widgets/Primitives/PopupItem.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/Stack.hpp"
#include "Core/GUI/Widgets/Compound/FileMenu.hpp"
#include "Core/GUI/Widgets/Compound/Popup.hpp"

#include "Editor/Widgets/Compound/ColorWheelCompound.hpp"
#include "Editor/Widgets/Compound/IconTabs.hpp"
#include "Editor/Widgets/Compound/Tab.hpp"
#include "Editor/Widgets/Compound/TabRow.hpp"
#include "Editor/Widgets/Compound/WindowButtons.hpp"
#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/Docking/DockBorder.hpp"
#include "Editor/Widgets/Docking/DockPreview.hpp"
#include "Editor/Widgets/Docking/DockWidget.hpp"
#include "Editor/Widgets/Screens/SplashScreen.hpp"
#include "Editor/Widgets/Popups/ProjectSelector.hpp"

#include "Editor/Editor.hpp"


namespace Lina
{
	SystemInitializationInfo Lina_GetInitInfo()
	{
		LinaGX::MonitorInfo monitor = LinaGX::Window::GetPrimaryMonitorInfo();

		const uint32 w = monitor.size.x / 4;
		const uint32 h = static_cast<uint32>(static_cast<float>(w) * (static_cast<float>(monitor.size.y) / static_cast<float>(monitor.size.x)));

		return SystemInitializationInfo{
			.appName			 = "Lina Editor",
			.windowWidth		 = w,
			.windowHeight		 = h,
			.windowStyle		 = LinaGX::WindowStyle::BorderlessApplication,
			.appDelegate		 = new Lina::Editor::EditorApplicationDelegate(),
			.resourceManagerMode = Lina::ResourceManagerMode::File,
		};
	}
} // namespace Lina

namespace Lina::Editor
{
	void EditorApplicationDelegate::SetupPlatform(Application* app)
	{
		Theme::GetDef().iconFont			  = ICON_FONT_SID;
		Theme::GetDef().defaultFont			  = DEFAULT_FONT_SID;
		Theme::GetDef().alternativeFont		  = DEFAULT_FONT_SID;
		Theme::GetDef().iconDropdown		  = ICON_ARROW_DOWN;
		Theme::GetDef().iconSliderHandle	  = ICON_CIRCLE_FILLED;
		Theme::GetDef().iconColorWheelPointer = ICON_CIRCLE;

		Theme::SetWidgetChunkCount(GetTypeID<Button>(), 150);
		Theme::SetWidgetChunkCount(GetTypeID<Checkbox>(), 150);
		Theme::SetWidgetChunkCount(GetTypeID<ColorField>(), 150);
		Theme::SetWidgetChunkCount(GetTypeID<ColorWheel>(), 3);
		Theme::SetWidgetChunkCount(GetTypeID<Dropdown>(), 150);
		Theme::SetWidgetChunkCount(GetTypeID<InputField>(), 150);
		Theme::SetWidgetChunkCount(GetTypeID<Icon>(), 150);
		Theme::SetWidgetChunkCount(GetTypeID<Text>(), 150);
		Theme::SetWidgetChunkCount(GetTypeID<Slider>(), 150);
		Theme::SetWidgetChunkCount(GetTypeID<PopupItem>(), 150);
		Theme::SetWidgetChunkCount(GetTypeID<DirectionalLayout>(), 150);
		Theme::SetWidgetChunkCount(GetTypeID<Stack>(), 150);
		Theme::SetWidgetChunkCount(GetTypeID<FileMenu>(), 5);
		Theme::SetWidgetChunkCount(GetTypeID<Popup>(), 50);
		Theme::SetWidgetChunkCount(GetTypeID<ColorWheelCompound>(), 1);
		Theme::SetWidgetChunkCount(GetTypeID<IconTabs>(), 50);
		Theme::SetWidgetChunkCount(GetTypeID<Tab>(), 50);
		Theme::SetWidgetChunkCount(GetTypeID<TabRow>(), 25);
		Theme::SetWidgetChunkCount(GetTypeID<WindowButtons>(), 15);
		Theme::SetWidgetChunkCount(GetTypeID<DockArea>(), 25);
		Theme::SetWidgetChunkCount(GetTypeID<DockBorder>(), 25);
		Theme::SetWidgetChunkCount(GetTypeID<DockPreview>(), 25);
		Theme::SetWidgetChunkCount(GetTypeID<DockWidget>(), 25);
		Theme::SetWidgetChunkCount(GetTypeID<SplashScreen>(), 1);
		Theme::SetWidgetChunkCount(GetTypeID<ProjectSelector>(), 1);
        
        m_app = app;
        m_editor = new Editor(m_app->GetSystem());
	}

	void EditorApplicationDelegate::RegisterAppResources(ResourceManager& rm)
	{
		ApplicationDelegate::RegisterAppResources(rm);

		Vector<ResourceIdentifier> resources;

		// Priority
		resources.push_back(ResourceIdentifier(ICON_FONT_PATH, GetTypeID<Font>(), 0, true, ResourceTag::Priority));
		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/LinaLogoTitle.png", GetTypeID<Texture>(), 0, true, ResourceTag::Priority));

		for (auto& r : resources)
			r.sid = TO_SID(r.path);

		rm.RegisterAppResources(resources);
	}

	bool EditorApplicationDelegate::FillResourceCustomMeta(StringID sid, OStream& stream)
	{
		if (ApplicationDelegate::FillResourceCustomMeta(sid, stream))
			return true;

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

} // namespace Lina::Editor
