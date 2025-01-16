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

#include "Editor/Widgets/Panel/PanelDetails.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/Compound/EntityBrowser.hpp"
#include "Editor/Widgets/Compound/EntityDetails.hpp"
#include "Editor/Widgets/Panel/PanelWorld.hpp"
#include "Editor/Graphics/EditorWorldRenderer.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"

namespace Lina::Editor
{
	void PanelDetails::Construct()
	{
		m_editor = Editor::Get();

		DirectionalLayout* vertical = m_manager->Allocate<DirectionalLayout>();
		vertical->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		vertical->GetProps().direction				   = DirectionOrientation::Vertical;
		vertical->GetWidgetProps().childPadding		   = Theme::GetDef().baseIndent;
		vertical->GetWidgetProps().childMargins.top	   = Theme::GetDef().baseIndent;
		vertical->GetWidgetProps().childMargins.bottom = Theme::GetDef().baseIndent;
		AddChild(vertical);

		m_entityDetails = m_manager->Allocate<EntityDetails>("EntityDetails");
		m_entityDetails->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_entityDetails->SetAlignedPos(Vector2::Zero);
		m_entityDetails->SetAlignedSize(Vector2::One);
		AddChild(m_entityDetails);

		Panel* panel = m_editor->GetWindowPanelManager().FindPanelOfType(PanelType::World, 0);
		if (panel)
		{
			EntityWorld* w = static_cast<PanelWorld*>(panel)->GetWorld();
			SetWorld(w);
		}

		m_editor->GetWorldManager().AddListener(this);
	}

	void PanelDetails::Destruct()
	{
		m_editor->GetWorldManager().RemoveListener(this);
	}

	void PanelDetails::PreTick()
	{
	}

	void PanelDetails::Tick(float dt)
	{
	}

	void PanelDetails::SetWorld(EntityWorld* world)
	{
		m_entityDetails->SetWorld(world);
		m_entityDetails->RefreshDetails();
	}

	void PanelDetails::OnWorldManagerEntitySelectionChanged(EntityWorld* w, const Vector<Entity*>& entities, StringID source)
	{
		if (w->GetID() == 0)
			return;
		m_entityDetails->OnEntitySelectionChanged(entities);
	}

	void PanelDetails::OnWorldManagerEntityHierarchyChanged(EntityWorld* w)
	{
		if (w->GetID() == 0)
			return;
		m_entityDetails->RefreshDetails();
	}

	void PanelDetails::OnWorldManagerEntityPhysicsSettingsChanged(EntityWorld* w)
	{
		if (w->GetID() == 0)
			return;
		m_entityDetails->RefreshDetails();
	}

	void PanelDetails::OnWorldManagerComponentsDataChanged(EntityWorld* w)
	{
		if (w->GetID() == 0)
			return;
		m_entityDetails->RefreshDetails();
	}
} // namespace Lina::Editor
