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

#include "Editor/EditorWorldManager.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/Panel/PanelWorld.hpp"
#include "Core/Application.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Common/FileSystem/FileSystem.hpp"

namespace Lina::Editor
{
	void EditorWorldManager::Initialize(Editor* editor)
	{
		m_editor = editor;
	}

	void EditorWorldManager::Shutdown()
	{
	}

	void EditorWorldManager::OpenWorld(ResourceID id)
	{
		const String resourcePath = m_editor->GetProjectManager().GetProjectData()->GetResourcePath(id);

		if (!FileSystem::FileOrPathExists(resourcePath))
		{
			LINA_ERR("Can not open world with id {0}", id);
			return;
		}

		PanelWorld* panel = static_cast<PanelWorld*>(m_editor->GetWindowPanelManager().OpenPanel(PanelType::World, 0, nullptr));
		panel->DestroyWorld();
		panel->CreateWorld(resourcePath);

		/*
		Widget* lockRoot = m_editor->GetWindowPanelManager().LockAllForegrounds(panel->GetWindow(), [](Widget* owner) -> Widget* { return CommonWidgets::BuildSimpleForegroundLockText(owner, Locale::GetStr(LocaleStr::WorkInProgressInAnotherWindow)); });
		Vector<CommonWidgets::GenericPopupButton> buttons = {
			{
				.title = Locale::GetStr(LocaleStr::Yes),
				.onPressed =
					[]() {

					},
			},
			{
				.title	   = Locale::GetStr(LocaleStr::No),
				.onPressed = [this]() { m_editor->GetWindowPanelManager().UnlockAllForegrounds(); },
			},
		};
		lockRoot->AddChild(CommonWidgets::BuildGenericPopupWithButtons(lockRoot, "Moikka", buttons));
		*/
	}

} // namespace Lina::Editor
