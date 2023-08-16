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

#include "GUI/EditorLayoutManager.hpp"
#include "Core/Editor.hpp"
#include "System/ISystem.hpp"
#include "Serialization/Serialization.hpp"
#include "Serialization/StringSerialization.hpp"
#include "FileSystem/FileSystem.hpp"
#include "GUI/Drawers/GUIDrawerBase.hpp"
#include "Data/CommonData.hpp"
#include "GUI/Nodes/Panels/GUIPanelFactory.hpp"
#include "GUI/Nodes/Panels/GUINodePanel.hpp"
#include "GUI/Nodes/Docking/GUINodeDockArea.hpp"
#include "Math/Rect.hpp"
#include "Graphics/Core/LGXWrapper.hpp"
#include "Graphics/Core/CommonGraphics.hpp"

namespace Lina::Editor
{
#define SAVED_LAYOUT_FILENAME "LinaEditorLayout.linasettings"

	EditorLayoutManager::EditorLayoutManager(Editor* editor) : m_editor(editor)
	{
		m_lgxWrapper = m_editor->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper);
	}

	void EditorLayoutManager::SaveCurrentLayout()
	{
		const auto& windows	   = m_lgxWrapper->GetWindowManager()->GetWindows();
		const auto& guiDrawers = m_editor->GetGUIDrawers();

		OStream stream;
		stream.CreateReserve(100);

		// omitting payload window
		const uint32 windowsSize = static_cast<uint32>(windows.size()) - 1;
		stream << windowsSize;

		stream << Editor::s_childWindowCtr;

		for (auto [sid, w] : windows)
		{
			if (sid == EDITOR_PAYLOAD_WINDOW_SID)
				continue;

			LayoutWindow layoutWindow = LayoutWindow{
				.title = w->GetTitle().c_str(),
				.sid   = sid,
				.pos   = w->GetPosition(),
				.size  = w->GetSize(),
			};

			SerializeLayoutWindow(stream, layoutWindow);

			auto guiDrawer = guiDrawers.at(sid);
			guiDrawer->SaveToStream(stream);
		}

		Serialization::SaveToFile(SAVED_LAYOUT_FILENAME, stream);
		stream.Destroy();
	}

	void EditorLayoutManager::LoadSavedLayout()
	{

		if (!FileSystem::FileExists(SAVED_LAYOUT_FILENAME))
		{
			LoadDefaultLayout();
			return;
		}
		else
		{
			ClearLayout();
		}

		IStream stream		= Serialization::LoadFromFile(SAVED_LAYOUT_FILENAME);
		uint32	windowsSize = 0;
		stream >> windowsSize;

		stream >> Editor::s_childWindowCtr;

		const auto& monitors = m_lgxWrapper->GetWindowManager()->GetMonitors();

		for (uint32 i = 0; i < windowsSize; i++)
		{
			auto& guiDrawers = m_editor->GetGUIDrawers();

			LayoutWindow w;
			DeserializeLayoutWindow(stream, w);

			if (w.sid != LINA_MAIN_SWAPCHAIN)
			{
				bool fitsMonitor = false;

				for (const auto& monitor : monitors)
				{
					const Recti monitorRect = Recti(monitor.workTopLeft, Vector2(monitor.workTopLeft.x + monitor.workArea.x, monitor.workTopLeft.y + monitor.workArea.y));
					if (monitorRect.IsPointInside(w.pos))
					{
						fitsMonitor = true;
						break;
					}
				}

				m_editor->CreateChildWindow(w.sid, w.title, fitsMonitor ? w.pos : Vector2i::Zero, w.size);
			}

			GUIDrawerBase* guiDrawer = nullptr;
			guiDrawer				 = guiDrawers.at(w.sid);
			guiDrawer->LoadFromStream(stream);
		}

		stream.Destroy();
	}

	void EditorLayoutManager::LoadDefaultLayout()
	{
		ClearLayout();

		const auto& guiDrawers = m_editor->GetGUIDrawers();
		auto*		guiDrawer  = guiDrawers.at(LINA_MAIN_SWAPCHAIN);
		auto*		dockArea   = guiDrawer->GetDockAreas()[0];

		const String entitiesName		 = PANEL_TO_NAME_MAP.at(EditorPanel::Entities);
		const String levelName			 = PANEL_TO_NAME_MAP.at(EditorPanel::Level);
		const String contentBrowserName	 = PANEL_TO_NAME_MAP.at(EditorPanel::ContentBrowser);
		auto*		 entPanel			 = GUIPanelFactory::CreatePanel(EditorPanel::Entities, dockArea, entitiesName, TO_SID(entitiesName));
		auto*		 levelPanel			 = GUIPanelFactory::CreatePanel(EditorPanel::Level, dockArea, levelName, TO_SID(levelName));
		auto*		 contentBrowserPanel = GUIPanelFactory::CreatePanel(EditorPanel::ContentBrowser, dockArea, contentBrowserName, TO_SID(contentBrowserName));
		dockArea->AddPanel(levelPanel);

		guiDrawer->SplitDockArea(dockArea, DockSplitType::Left, {entPanel}, 0.15f);
		guiDrawer->SplitDockArea(dockArea, DockSplitType::Down, {contentBrowserPanel}, 0.25f);

		SaveCurrentLayout();
	}

	void EditorLayoutManager::SerializeLayoutWindow(OStream& stream, const LayoutWindow& l)
	{
		stream << l.sid;
		l.pos.SaveToStream(stream);
		l.size.SaveToStream(stream);
		StringSerialization::SaveToStream(stream, l.title);
	}

	void EditorLayoutManager::DeserializeLayoutWindow(IStream& stream, LayoutWindow& l)
	{
		stream >> l.sid;
		l.pos.LoadFromStream(stream);
		l.size.LoadFromStream(stream);
		StringSerialization::LoadFromStream(stream, l.title);
	}

	void EditorLayoutManager::ClearLayout()
	{
		// Prepare editor for loading a layout, e.g remove all windows & extra dock areas
		{
			m_editor->CloseAllChildWindows();

			auto		mainGuiDrawer = m_editor->GetGUIDrawers().at(LINA_MAIN_SWAPCHAIN);
			const auto& allDocksMain  = mainGuiDrawer->GetDockAreas();

			for (auto d : allDocksMain)
				d->RemoveAllPanels();

			Vector<GUINodeDockArea*> areas = allDocksMain;
			const uint32			 sz	   = static_cast<uint32>(areas.size());

			for (uint32 i = 0; i < sz; i++)
			{
				if (i != 0)
					mainGuiDrawer->RemoveDockArea(areas[i]);
			}
		}
	}

} // namespace Lina::Editor
