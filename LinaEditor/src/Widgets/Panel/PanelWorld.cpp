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
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Editor/Graphics/EditorWorldRenderer.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Serialization/Serialization.hpp"
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
	}

	void PanelWorld::Destruct()
	{
		DestroyWorld();
	}

	void PanelWorld::CreateWorld(const String& resourcePath)
	{
		LINA_ASSERT(m_world == nullptr, "");

		m_world = new EntityWorld(0, "");
		m_editor->GetApp()->GetWorldProcessor().AddWorld(m_world);
		m_worldRenderer		  = new WorldRenderer(&m_editor->GetApp()->GetGfxContext(), &m_editor->GetApp()->GetResourceManager(), m_world, Vector2ui(4, 4), "WorldRenderer: " + m_world->GetName());
		m_editorWorldRenderer = new EditorWorldRenderer(m_editor, m_editor->GetApp()->GetLGX(), m_worldRenderer);
		m_editor->GetEditorRenderer().AddWorldRenderer(m_worldRenderer, m_editorWorldRenderer);

		IStream stream = Serialization::LoadFromFile(resourcePath.c_str());
		m_world->LoadFromStream(stream);
		stream.Destroy();

		HashSet<ResourceID> defaultResources = {

		};

		m_world->LoadMissingResources(m_editor->GetApp()->GetResourceManager(), m_editor->GetProjectManager().GetProjectData(), defaultResources, m_world->GetID());

		m_worldDisplayer->DisplayWorld(m_worldRenderer, m_editorWorldRenderer, WorldCameraType::Orbit);
	}

	void PanelWorld::DestroyWorld()
	{
		if (m_world == nullptr)
			return;

		const ResourceID space = m_world->GetID();

		m_editor->GetEditorRenderer().RemoveWorldRenderer(m_worldRenderer);
		delete m_worldRenderer;
		delete m_editorWorldRenderer;

		m_editor->GetApp()->GetWorldProcessor().RemoveWorld(m_world);
		delete m_world;

		m_world			= nullptr;
		m_worldRenderer = nullptr;

		m_worldDisplayer->DisplayWorld(nullptr, nullptr, WorldCameraType::FreeMove);

		m_editor->GetApp()->GetResourceManager().UnloadResourceSpace(space);
	}

} // namespace Lina::Editor
