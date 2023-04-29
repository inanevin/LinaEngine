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

#include "Commands/EditorCommandEntity.hpp"
#include "Core/Editor.hpp"
#include "System/ISystem.hpp"
#include "World/Level/LevelManager.hpp"
#include "World/Level/Level.hpp"
#include "World/Core/EntityWorld.hpp"

namespace Lina::Editor
{
	EditorCommandSelectEntity::EditorCommandSelectEntity(EditorCommandManager* manager, Entity* previous, Entity* current) : EditorCommand(manager), m_previous(previous), m_current(current)
	{
		m_world = m_editor->GetSystem()->CastSubsystem<LevelManager>(SubsystemType::LevelManager)->GetCurrentLevel()->GetWorld();
		m_world->AddListener(this);
	}

	EditorCommandSelectEntity::~EditorCommandSelectEntity()
	{
		if (m_world)
			m_world->RemoveListener(this);
	}

	void EditorCommandSelectEntity::OnGameEvent(GameEvent eventType, const Event& ev)
	{
		if (eventType & EVG_EntityDestroyed)
		{
			if (m_previous == ev.pParams[0])
			{
				m_previous = nullptr;
			}
		}
		else if (eventType & EVG_End)
		{
			m_world->RemoveListener(this);
			m_world = nullptr;
		}
	}

	void EditorCommandSelectEntity::Execute(void* userData)
	{
		Event ev	  = Event();
		ev.pParams[0] = m_current;
		m_editor->DispatchEvent(EVE_SelectEntity, ev);
	}

	void EditorCommandSelectEntity::Undo()
	{
		Event ev = Event();
		m_editor->DispatchEvent(EVE_UnselectCurrentEntity, ev);

		if (m_previous)
		{
			ev.pParams[0] = m_previous;
			m_editor->DispatchEvent(EVE_SelectEntity, ev);
		}
	}
} // namespace Lina::Editor
