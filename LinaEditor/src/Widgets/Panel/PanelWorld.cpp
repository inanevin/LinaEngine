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
#include "Editor/Widgets/Compound/EntityBrowser.hpp"
#include "Editor/Widgets/Compound/EntityDetails.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Editor/Graphics/EditorWorldRenderer.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
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
	}

	void PanelWorld::Destruct()
	{
		m_editor->GetWorldManager().CloseWorld(m_world);
		m_editor->GetWorldManager().RemoveListener(this);
	}

	void PanelWorld::OnWorldManagerOpenedWorld(EditorWorldRenderer* wr)
	{
		EntityWorld* world = wr->GetWorldRenderer()->GetWorld();
		if (world->GetID() == 0)
			return;
		m_world = wr->GetWorldRenderer()->GetWorld();
		m_worldDisplayer->DisplayWorld(wr, WorldCameraType::FreeMove);
	}

	void PanelWorld::OnWorldManagerClosingWorld(EditorWorldRenderer* wr)
	{
		if (wr->GetWorldRenderer()->GetWorld() != m_world)
			return;
		m_world = nullptr;
		m_worldDisplayer->DisplayWorld(nullptr, WorldCameraType::FreeMove);
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

} // namespace Lina::Editor
