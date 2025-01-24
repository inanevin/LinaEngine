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

#include "Editor/Actions/EditorActionWorld.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Widgets/World/WorldController.hpp"
#include "Editor/World/EditorWorldUtility.hpp"
#include "Editor/Widgets/Panel/PanelWorld.hpp"
#include "Editor/Widgets/World/WorldDisplayer.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/World/Entity.hpp"
#include "Core/World/Component.hpp"
#include "Core/Physics/PhysicsWorld.hpp"
#include "Core/Application.hpp"

namespace Lina::Editor
{

	EditorActionWorldSnappingChanged* EditorActionWorldSnappingChanged::Create(Editor* editor, EntityWorld* world, const WorldSnapSettings& prevOptions, const WorldSnapSettings& currentOptions)
	{
		EditorActionWorldSnappingChanged* action = new EditorActionWorldSnappingChanged();
		action->m_worldId						 = world->GetID();
		action->m_prevSettings					 = prevOptions;
		action->m_currentSettings				 = currentOptions;
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionWorldSnappingChanged::Execute(Editor* editor, ExecType type)
	{
		EntityWorld* world = editor->GetWorldManager().GetWorld(m_worldId);
		if (!world)
			return;

		Panel* panel = editor->GetWindowPanelManager().FindPanelOfType(PanelType::World, 0);
		if (!panel)
			return;

		WorldController* wc = static_cast<PanelWorld*>(panel)->GetDisplayer()->GetController();

		if (type == ExecType::Undo)
		{
			wc->SetSnappingSettings(m_prevSettings);
		}
		else if (type == ExecType::Redo)
		{
			wc->SetSnappingSettings(m_currentSettings);
		}
	}

	EditorActionWorldGfxChanged* EditorActionWorldGfxChanged::Create(Editor* editor, EntityWorld* world, const WorldGfxSettings& prevOptions, const WorldGfxSettings& currentOptions)
	{

		EditorActionWorldGfxChanged* action = new EditorActionWorldGfxChanged();
		action->m_worldId					= world->GetID();
		action->m_prevSettings				= prevOptions;
		action->m_currentSettings			= currentOptions;
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionWorldGfxChanged::Execute(Editor* editor, ExecType type)
	{
		EntityWorld* world = editor->GetWorldManager().GetWorld(m_worldId);
		if (!world)
			return;

		if (type == ExecType::Undo)
		{
			world->GetGfxSettings() = m_prevSettings;
		}
		else if (type == ExecType::Redo)
		{
			world->GetGfxSettings() = m_currentSettings;
		}

		world->LoadMissingResources(editor->GetApp()->GetResourceManager(), editor->GetProjectManager().GetProjectData(), {});
	}

	EditorActionWorldPhysicsSettingsChanged* EditorActionWorldPhysicsSettingsChanged::Create(Editor* editor, EntityWorld* world, const WorldPhysicsSettings& prevOptions, const WorldPhysicsSettings& currentOptions)
	{

		EditorActionWorldPhysicsSettingsChanged* action = new EditorActionWorldPhysicsSettingsChanged();
		action->m_worldId								= world->GetID();
		action->m_prevSettings							= prevOptions;
		action->m_currentSettings						= currentOptions;
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionWorldPhysicsSettingsChanged::Execute(Editor* editor, ExecType type)
	{
		EntityWorld* world = editor->GetWorldManager().GetWorld(m_worldId);
		if (!world)
			return;

		if (type == ExecType::Undo)
		{
			world->GetPhysicsSettings() = m_prevSettings;
		}
		else if (type == ExecType::Redo)
		{
			world->GetPhysicsSettings() = m_currentSettings;
		}

		world->GetPhysicsWorld()->SetPhysicsSettings(world->GetPhysicsSettings());
	}

} // namespace Lina::Editor
