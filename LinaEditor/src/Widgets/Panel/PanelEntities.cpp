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

#include "Editor/Widgets/Panel/PanelEntities.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/Compound/EntityBrowser.hpp"
#include "Editor/Graphics/EditorWorldRenderer.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Editor/Editor.hpp"

namespace Lina::Editor
{
	void PanelEntities::Construct()
	{
		m_editor = Editor::Get();

		m_browser = m_manager->Allocate<EntityBrowser>("EntityBrowser");
		m_browser->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_browser->SetAlignedPos(Vector2::Zero);
		m_browser->SetAlignedSize(Vector2(1.0f, 1.0f));
		AddChild(m_browser);

		EntityWorld* w = m_editor->GetWorldManager().GetFirstGameWorld();

		if (w)
			m_browser->SetWorld(w);

		m_editor->GetWorldManager().AddListener(this);
	}

	void PanelEntities::Destruct()
	{
		m_editor->GetWorldManager().RemoveListener(this);
	}

	void PanelEntities::PreTick()
	{
	}

	void PanelEntities::Tick(float dt)
	{
	}

	void PanelEntities::OnWorldManagerOpenedWorld(EditorWorldRenderer* wr)
	{
		EntityWorld* world = wr->GetWorldRenderer()->GetWorld();
		if (world->GetID() == 0)
			return;
		m_browser->SetWorld(wr->GetWorldRenderer()->GetWorld());
	}

	void PanelEntities::OnWorldManagerClosingWorld(EditorWorldRenderer* wr)
	{
		EntityWorld* world = wr->GetWorldRenderer()->GetWorld();
		if (world->GetID() == 0)
			return;
		m_browser->SetWorld(nullptr);
	}

	void PanelEntities::OnWorldManagerEntitySelectionChanged(EntityWorld* w, const Vector<Entity*>& entities, StringID source)
	{
		if (w->GetID() == 0)
			return;
		m_browser->OnEntitySelectionChanged(entities, source == 0);
	}

	void PanelEntities::OnWorldManagerEntityHierarchyChanged(EntityWorld* w)
	{
		if (w->GetID() == 0)
			return;
		m_browser->RefreshEntities();
	}
} // namespace Lina::Editor
