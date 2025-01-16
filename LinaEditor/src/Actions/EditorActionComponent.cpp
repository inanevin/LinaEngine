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

#include "Editor/Actions/EditorActionComponent.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Widgets/World/WorldController.hpp"
#include "Editor/World/EditorWorldUtility.hpp"
#include "Core/World/Entity.hpp"
#include "Core/World/Component.hpp"
#include "Core/Application.hpp"

namespace Lina::Editor
{

	EditorActionComponentChanged* EditorActionComponentChanged::Create(Editor* editor, uint64 worldId, const Vector<Entity*>& entities, const Vector<TypeID>& comps, const Vector<OStream>& previousBuffers)
	{
		if (!(entities.size() == comps.size() && comps.size() == previousBuffers.size()))
			return;

		EditorActionComponentChanged* action = new EditorActionComponentChanged();
		action->m_entities.reserve(entities.size());
		action->m_worldId = worldId;

		EditorWorldManager::WorldData& wd = editor->GetWorldManager().GetWorldData(editor->GetWorldManager().GetWorld(worldId));

		uint32 i = 0;
		for (Entity* e : entities)
		{
			if (e != nullptr)
				action->m_entities.push_back(e->GetGUID());

			Component* c = wd.world->GetComponent(e, comps.at(i));
			OStream	   stream;
			c->SaveToStream(stream);
			action->m_currentStreams.push_back(stream);
			i++;
		}

		action->m_components = comps;

		for (const OStream& stream : previousBuffers)
		{
			OStream prevStream;
			prevStream.WriteRaw(stream.GetDataRaw(), stream.GetCurrentSize());
			action->m_previousStreams.push_back(prevStream);
		}
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionComponentChanged::Execute(Editor* editor, ExecType type)
	{
		EntityWorld* world = editor->GetWorldManager().GetWorld(m_worldId);
		if (!world)
			return;

		EditorWorldManager::WorldData& wd = editor->GetWorldManager().GetWorldData(world);

		Vector<Component*> comps;
		comps.resize(m_previousStreams.size());

		uint32 i = 0;
		for (EntityID guid : m_entities)
		{
			Entity*	   e = wd.world->GetEntity(m_entities.at(i));
			Component* c = wd.world->GetComponent(e, m_components.at(i));
			comps[i]	 = c;
		}

		if (type == ExecType::Undo)
		{
			i = 0;
			for (OStream& stream : m_previousStreams)
			{
				IStream istream;
				istream.Create(stream.GetDataRaw(), stream.GetCurrentSize());
				comps.at(i)->LoadFromStream(istream);
				istream.Destroy();
				i++;
			}
		}
		else if (type == ExecType::Redo)
		{
			i = 0;
			for (OStream& stream : m_currentStreams)
			{
				IStream istream;
				istream.Create(stream.GetDataRaw(), stream.GetCurrentSize());
				comps.at(i)->LoadFromStream(istream);
				istream.Destroy();
				i++;
			}
		}

		for (Component* c : comps)
		{
			wd.world->LoadMissingResources(editor->GetApp()->GetResourceManager(), editor->GetProjectManager().GetProjectData(), {});
			c->StoreReferences();
		}

		if (type == ExecType::Redo || type == ExecType::Undo)
			editor->GetWorldManager().BroadcastComponentsChanged(wd.world);
	}

} // namespace Lina::Editor
