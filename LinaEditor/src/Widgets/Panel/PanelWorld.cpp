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

#include "Editor/Widgets/Panel/PanelWorld.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/World/WorldDisplayer.hpp"
#include "Editor/Widgets/World/WorldController.hpp"
#include "Editor/Widgets/Panel/PanelResourceBrowser.hpp"
#include "Editor/Widgets/Panel/PanelEntities.hpp"
#include "Editor/Widgets/Panel/PanelDetails.hpp"
#include "Editor/Widgets/Compound/EntityBrowser.hpp"
#include "Editor/Widgets/Compound/EntityDetails.hpp"
#include "Editor/World/EditorWorldUtility.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Editor/Graphics/EditorWorldRenderer.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Math/Math.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Physics/PhysicsWorld.hpp"
#include "Core/Application.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void PanelWorld::Construct()
	{
		Panel::Construct();
		m_editor = Editor::Get();

		GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);

		m_worldDisplayer = m_manager->Allocate<WorldDisplayer>("WorldDisplayer");
		m_worldDisplayer->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_worldDisplayer->SetAlignedPos(Vector2::Zero);
		m_worldDisplayer->SetAlignedSize(Vector2::One);
		m_worldDisplayer->GetProps().noWorldText = Locale::GetStr(LocaleStr::NoWorldInstalled);

		AddChild(m_worldDisplayer);

		m_editor->GetWorldManager().AddListener(this);

		const ResourceID id = m_editor->GetSettings().GetParams().GetParamResourceID("LastWorld"_hs);
		m_worldToOpen		= id;
		m_openWorld			= true;

		GetWidgetProps().debugName = Locale::GetStr(LocaleStr::World) + ": None";
		RefreshTab();
	}

	void PanelWorld::Destruct()
	{
		CloseWorld();
		m_editor->GetWorldManager().RemoveListener(this);
	}

	void PanelWorld::PreTick()
	{
		if (m_openWorld)
		{
			m_openWorld = false;
			OpenWorld(m_worldToOpen);
		}
	}

	void PanelWorld::OnWorldManagerEntitySelectionChanged(EntityWorld* w, const Vector<Entity*>& entities, StringID source)
	{
		if (w != m_world)
			return;

		m_worldDisplayer->GetController()->OnEntitySelectionChanged(entities);
	}

	void PanelWorld::OnWorldManagerEntityHierarchyChanged(EntityWorld* w)
	{
		if (w != m_world)
			return;
	}

	void PanelWorld::OpenWorld(ResourceID id)
	{
		const String path = m_editor->GetProjectManager().GetProjectData()->GetResourcePath(id);
		if (!FileSystem::FileOrPathExists(path))
			return;

		CloseWorld();

		EditorWorldRenderer* ewr = m_editor->GetWorldManager().CreateEditorWorld(id, m_lgxWindow);
		m_world					 = ewr->GetWorldRenderer()->GetWorld();
		ewr->GetWorldRenderer()->CreatePhysicsDebugRenderer();
		m_world->LoadFromFile(path);
		m_world->LoadMissingResources(m_editor->GetApp()->GetResourceManager(), m_editor->GetProjectManager().GetProjectData(), {});
		m_world->RefreshAllComponentReferences();

		GetWidgetProps().debugName = Locale::GetStr(LocaleStr::World) + ": " + m_world->GetName();
		RefreshTab();

		Panel* panelEntities = m_editor->GetWindowPanelManager().FindPanelOfType(PanelType::Entities, 0);
		Panel* panelDetails	 = m_editor->GetWindowPanelManager().FindPanelOfType(PanelType::Details, 0);

		if (panelEntities)
			static_cast<PanelEntities*>(panelEntities)->SetWorld(m_world);

		if (panelDetails)
			static_cast<PanelDetails*>(panelDetails)->SetWorld(m_world);

		m_editor->GetSettings().GetParams().SetParamResourceID("LastWorld"_hs, m_world->GetID());
		m_worldDisplayer->DisplayWorld(ewr, WorldCameraType::FreeMove);
	}

	void PanelWorld::CloseWorld()
	{
		if (!m_world)
			return;

		m_world->SaveToFileAsBinary(m_editor->GetProjectManager().GetProjectData()->GetResourcePath(m_world->GetID()));
		m_editor->GetWorldManager().DestroyEditorWorld(m_world);
		m_world = nullptr;

		GetWidgetProps().debugName = Locale::GetStr(LocaleStr::World) + ": None";
		RefreshTab();

		Panel* panelEntities = m_editor->GetWindowPanelManager().FindPanelOfType(PanelType::Entities, 0);
		Panel* panelDetails	 = m_editor->GetWindowPanelManager().FindPanelOfType(PanelType::Details, 0);

		if (panelEntities)
			static_cast<PanelEntities*>(panelEntities)->SetWorld(nullptr);

		if (panelDetails)
			static_cast<PanelDetails*>(panelDetails)->SetWorld(nullptr);
	}
} // namespace Lina::Editor
