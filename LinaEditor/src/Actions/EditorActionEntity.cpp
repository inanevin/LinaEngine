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

#include "Editor/Actions/EditorActionEntity.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Widgets/World/WorldController.hpp"
#include "Core/World/Entity.hpp"
#include "Core/World/Component.hpp"

namespace Lina::Editor
{

	EditorActionEntitySelection* EditorActionEntitySelection::Create(Editor* editor, uint64 worldId, const Vector<Entity*>& previousSelection, const Vector<Entity*>& currentSelection)
	{
		EditorActionEntitySelection* action = new EditorActionEntitySelection();

		action->m_prevSelected.reserve(previousSelection.size());
		action->m_selected.reserve(currentSelection.size());
		action->m_worldId = worldId;

		for (Entity* prev : previousSelection)
			action->m_prevSelected.push_back(prev->GetGUID());

		for (Entity* prev : currentSelection)
			action->m_selected.push_back(prev->GetGUID());

		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionEntitySelection::Execute(Editor* editor, ExecType type)
	{
		if (type == ExecType::Undo)
		{
			editor->GetWorldManager().OnActionEntitySelection(m_worldId, m_prevSelected);
		}
		else if (type == ExecType::Redo)
		{
			editor->GetWorldManager().OnActionEntitySelection(m_worldId, m_selected);
		}
	}

	EditorActionEntityTransform* EditorActionEntityTransform::Create(Editor* editor, uint64 worldId, const Vector<Entity*>& entities, const Vector<Transformation>& previousTransforms)
	{
		EditorActionEntityTransform* action = new EditorActionEntityTransform();

		for (Entity* e : entities)
		{
			action->m_entities.push_back(e->GetGUID());
			action->m_currentTransforms.push_back(e->GetTransform());
		}

		action->m_prevTransforms = previousTransforms;
		action->m_worldId		 = worldId;

		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionEntityTransform::Execute(Editor* editor, ExecType type)
	{
		if (type == ExecType::Undo)
		{
			editor->GetWorldManager().OnActionEntityTransform(m_worldId, m_entities, m_prevTransforms);
		}
		else if (type == ExecType::Redo)
		{
			editor->GetWorldManager().OnActionEntityTransform(m_worldId, m_entities, m_currentTransforms);
		}
	}

	EditorActionEntitiesCreated* EditorActionEntitiesCreated::Create(Editor* editor, EntityWorld* world, const Vector<Entity*>& entities)
	{
		EditorActionEntitiesCreated* action = new EditorActionEntitiesCreated();
		action->m_worldId					= world->GetID();

		const uint32 sz = static_cast<uint32>(entities.size());
		action->m_entityStream << sz;

		Vector<Component*> comps;
		for (Entity* e : entities)
		{
			action->m_guids.push_back(e->GetGUID());

			e->SaveToStream(action->m_entityStream);
			comps.resize(0);
			world->GetComponents(e, comps);

			const uint32 csz = static_cast<uint32>(comps.size());
			action->m_entityStream << csz;

			for (Component* c : comps)
			{
				action->m_entityStream << c->GetTID();
				c->SaveToStream(action->m_entityStream);
			}
		}

		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionEntitiesCreated::Execute(Editor* editor, ExecType type)
	{
		EntityWorld* world = editor->GetWorldManager().GetWorld(m_worldId);
		if (!world)
			return;

		if (type == ExecType::Undo)
		{
			editor->GetWorldManager().OnActionDeletingEntities(m_worldId, m_guids);

			for (EntityID e : m_guids)
				world->DestroyEntity(world->GetEntity(e));
		}
		else if (type == ExecType::Redo)
		{
			IStream stream;
			stream.Create(m_entityStream.GetDataRaw(), m_entityStream.GetCurrentSize());

			uint32 sz = 0;
			stream >> sz;

			for (uint32 i = 0; i < sz; i++)
			{
				Entity* e = world->CreateEntity("");
				e->LoadFromStream(stream);

				uint32 compsSize = 0;
				stream >> compsSize;

				for (uint32 j = 0; j < compsSize; j++)
				{
					TypeID tid = 0;
					stream >> tid;

					Component* c = world->AddComponent(e, tid);
					c->LoadFromStream(stream);
				}
			}

			stream.Destroy();
		}
	}

} // namespace Lina::Editor
