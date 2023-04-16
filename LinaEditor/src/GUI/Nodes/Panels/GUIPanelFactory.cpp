/*
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

#include "GUI/Nodes/Panels/GUIPanelFactory.hpp"
#include "GUI/Nodes/Panels/GUINodePanelContentBrowser.hpp"
#include "GUI/Nodes/Panels/GUINodePanelHierarchy.hpp"
#include "GUI/Nodes/Panels/GUINodePanelProperties.hpp"
#include "GUI/Nodes/Panels/GUINodePanelLevel.hpp"
#include "GUI/Nodes/Panels/GUINodePanelEntities.hpp"
#include "GUI/Nodes/Panels/GUINodePanelResourceViewer.hpp"
#include "GUI/Nodes/Docking/GUINodeDockArea.hpp"

namespace Lina::Editor
{
	GUINodePanel* GUIPanelFactory::CreatePanel(EditorPanel panelType, GUINodeDockArea* targetDockArea, const String& panelTitle, StringID sid)
	{
		GUINodePanel* panel = nullptr;

		switch (panelType)
		{
		case EditorPanel::ContentBrowser:
			panel = new GUINodePanelContentBrowser(targetDockArea->GetEditor(), targetDockArea->GetSwapchain(), targetDockArea->GetDrawOrder(), panelType, panelTitle, targetDockArea);
			break;
		case EditorPanel::Hierarchy:
			panel = new GUINodePanelHierarchy(targetDockArea->GetEditor(), targetDockArea->GetSwapchain(), targetDockArea->GetDrawOrder(), panelType, panelTitle, targetDockArea);
			break;
		case EditorPanel::Properties:
			panel = new GUINodePanelProperties(targetDockArea->GetEditor(), targetDockArea->GetSwapchain(), targetDockArea->GetDrawOrder(), panelType, panelTitle, targetDockArea);
			break;
		case EditorPanel::Level:
			panel = new GUINodePanelLevel(targetDockArea->GetEditor(), targetDockArea->GetSwapchain(), targetDockArea->GetDrawOrder(), panelType, panelTitle, targetDockArea);
			break;
		case EditorPanel::Entities:
			panel = new GUINodePanelEntities(targetDockArea->GetEditor(), targetDockArea->GetSwapchain(), targetDockArea->GetDrawOrder(), panelType, panelTitle, targetDockArea);
			break;
		case EditorPanel::DebugResourceView:
			panel = new GUINodePanelResourceViewer(targetDockArea->GetEditor(), targetDockArea->GetSwapchain(), targetDockArea->GetDrawOrder(), panelType, panelTitle, targetDockArea);
			break;
		default:
			LINA_NOTIMPLEMENTED;
		}

		panel->SetSID(sid);
		return panel;
	}
} // namespace Lina::Editor
