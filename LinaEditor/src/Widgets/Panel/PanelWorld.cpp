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
#include "Editor/Widgets/Panel/PanelResourceBrowser.hpp"
#include "Editor/Widgets/Compound/ResourceDirectoryBrowser.hpp"
#include "Editor/Graphics/GridRenderer.hpp"
#include "Editor/Graphics/GizmoRenderer.hpp"
#include "Editor/Graphics/MousePickRenderer.hpp"
#include "Editor/World/WorldUtility.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
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

		m_editor->GetWindowPanelManager().AddPayloadListener(this);
		AddChild(m_worldDisplayer);
	}

	void PanelWorld::Destruct()
	{
		m_editor->GetWindowPanelManager().RemovePayloadListener(this);
		DestroyWorld();
	}

	void PanelWorld::Tick(float delta)
	{
		// m_world = m_wm->GetMainWorld();
		// if (!m_world)
		// 	return;
		//
		// if (m_worldRenderer == nullptr)
		// 	m_worldRenderer = Editor::Get()->GetWorldRenderer(m_world);
		//
		// const Vector2ui size = Vector2ui(static_cast<uint32>(Math::CeilToInt(GetSizeX())), static_cast<uint32>(Math::CeilToInt(GetSizeY())));
		// m_worldRenderer->Resize(size);
	}

	void PanelWorld::CreateWorld(const String& resourcePath)
	{
		LINA_ASSERT(m_world == nullptr, "");

		m_world = new EntityWorld(0, "");
		m_editor->GetApp()->GetWorldProcessor().AddWorld(m_world);
		m_worldRenderer		= new WorldRenderer(&m_editor->GetApp()->GetGfxContext(), &m_editor->GetApp()->GetResourceManager(), m_world, Vector2ui(4, 4), "WorldRenderer: " + m_world->GetName());
		m_gizmoRenderer		= new GizmoRenderer(m_editor, m_editor->GetApp()->GetLGX(), m_world, m_worldRenderer, m_resourceManager);
		m_gridRenderer		= new GridRenderer(m_editor, m_editor->GetApp()->GetLGX(), m_world, m_worldRenderer, m_resourceManager);
		m_mousePickRenderer = new MousePickRenderer(m_editor, m_editor->GetApp()->GetLGX(), m_world, m_worldRenderer, m_resourceManager);
		m_worldRenderer->AddFeatureRenderer(m_gridRenderer);
		m_worldRenderer->AddFeatureRenderer(m_gizmoRenderer);
		m_worldRenderer->AddFeatureRenderer(m_mousePickRenderer);

		m_editor->GetEditorRenderer().AddWorldRenderer(m_worldRenderer);

		IStream stream = Serialization::LoadFromFile(resourcePath.c_str());
		m_world->LoadFromStream(stream);
		stream.Destroy();

		HashSet<ResourceID> defaultResources = {
			EDITOR_MODEL_GIZMO_TRANSLATE_ID,
			EDITOR_MODEL_GIZMO_ROTATE_ID,
			EDITOR_MODEL_GIZMO_SCALE_ID,
		};

		m_world->LoadMissingResources(m_editor->GetApp()->GetResourceManager(), m_editor->GetProjectManager().GetProjectData(), defaultResources, m_world->GetID());

		m_worldDisplayer->DisplayWorld(m_worldRenderer, WorldDisplayer::WorldCameraType::Orbit);
	}

	void PanelWorld::DestroyWorld()
	{
		if (m_world == nullptr)
			return;

		const ResourceID space = m_world->GetID();

		m_editor->GetEditorRenderer().RemoveWorldRenderer(m_worldRenderer);
		delete m_worldRenderer;
		delete m_gridRenderer;
		delete m_gizmoRenderer;
		delete m_mousePickRenderer;

		m_editor->GetApp()->GetWorldProcessor().RemoveWorld(m_world);
		delete m_world;

		m_world				= nullptr;
		m_worldRenderer		= nullptr;
		m_gridRenderer		= nullptr;
		m_gizmoRenderer		= nullptr;
		m_mousePickRenderer = nullptr;

		m_worldDisplayer->DisplayWorld(nullptr, WorldDisplayer::WorldCameraType::FreeMove);

		m_editor->GetApp()->GetResourceManager().UnloadResourceSpace(space);
	}

	void PanelWorld::SelectEntity(Entity* e)
	{
		m_selection.push_back(e);
		m_gizmoRenderer->SetSelectedEntity(e);
	}

	void PanelWorld::OnPayloadStarted(PayloadType type, Widget* payload)
	{
		if (type != PayloadType::Resource)
			return;

		if (m_world == nullptr)
			return;

		m_worldDisplayer->GetWidgetProps().colorOutline = Theme::GetDef().accentPrimary0;
	}

	void PanelWorld::OnPayloadEnded(PayloadType type, Widget* payload)
	{
		if (type != PayloadType::Resource)
			return;

		if (m_world == nullptr)
			return;

		m_worldDisplayer->GetWidgetProps().colorOutline = Theme::GetDef().outlineColorBase;
	}

	bool PanelWorld::OnPayloadDropped(PayloadType type, Widget* payload)
	{
		if (type != PayloadType::Resource)
			return false;

		if (m_world == nullptr)
			return false;

		if (!m_worldDisplayer->GetIsHovered())
			return false;

		Panel* panel = m_editor->GetWindowPanelManager().FindPanelOfType(PanelType::ResourceBrowser, 0);

		if (panel == nullptr)
			return false;

		const Vector<ResourceDirectory*>& payloadItems = static_cast<PanelResourceBrowser*>(panel)->GetBrowser()->GetPayloadItems();

		HashSet<ResourceID> resources;

		Entity* last = nullptr;

		for (ResourceDirectory* dir : payloadItems)
		{
			if (dir->resourceTID == GetTypeID<Model>())
			{
				WorldUtility::LoadModelAndMaterials(m_editor, dir->resourceID, m_world->GetID());

				Model*	model  = m_resourceManager->GetResource<Model>(dir->resourceID);
				Entity* entity = WorldUtility::AddModelToWorld(m_world, model, model->GetMeta().materials);
				m_world->LoadMissingResources(*m_resourceManager, m_editor->GetProjectManager().GetProjectData(), {}, m_world->GetID());

				const Vector2 mp	= m_lgxWindow->GetMousePosition() - m_worldDisplayer->GetStartFromMargins();
				const Vector2 size	= m_worldDisplayer->GetEndFromMargins() - m_worldDisplayer->GetStartFromMargins();
				const Vector3 point = Camera::ScreenToWorld(m_world->GetWorldCamera(), mp, size, 0.97f);
				entity->SetPosition(point);

				last = entity;
			}
		}

		SelectEntity(last);

		return true;
	}

	bool PanelWorld::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (!m_worldDisplayer->GetIsHovered())
			return false;

		if (button == LINAGX_MOUSE_0 && act == LinaGX::InputAction::Pressed)
		{
			const Vector2 mp = m_lgxWindow->GetMousePosition() - m_worldDisplayer->GetStartFromMargins();
			m_mousePickRenderer->PickEntity(mp);
		}

		return false;
	}

} // namespace Lina::Editor
