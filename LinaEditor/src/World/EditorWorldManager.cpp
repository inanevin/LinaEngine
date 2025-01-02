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

#include "Editor/World/EditorWorldManager.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/World/WorldController.hpp"
#include "Editor/Graphics/EditorWorldRenderer.hpp"

#include "Core/Application.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Editor/Actions/EditorActionEntity.hpp"
#include "Editor/World/WorldUtility.hpp"

namespace Lina::Editor
{
	void EditorWorldManager::Initialize(Editor* editor)
	{
		m_editor = editor;
	}

	void EditorWorldManager::Shutdown()
	{
	}

	void EditorWorldManager::Tick(float delta)
	{
		if (m_worlds.size() == 1)
		{
			const WorldData& data = m_worlds.at(0);
			data.world->Tick(delta);
		}
		else
		{
			Taskflow tf;
			tf.for_each_index(0, static_cast<int>(m_worlds.size()), 1, [&](int i) {
				const WorldData& data = m_worlds.at(i);
				data.world->Tick(delta);
			});
			m_executor.RunAndWait(tf);
		}
	}

	void EditorWorldManager::AddListener(EditorWorldManagerListener* listener)
	{
		m_listeners.push_back(listener);
	}

	void EditorWorldManager::RemoveListener(EditorWorldManagerListener* listener)
	{
		m_listeners.erase(linatl::find(m_listeners.begin(), m_listeners.end(), listener));
	}

	void EditorWorldManager::SelectEntity(EntityWorld* world, Entity* e, bool clearOthers)
	{
		WorldData& wd = GetWorldData(world);

		const Vector<Entity*> prev = wd.selectedEntities;

		if (clearOthers)
			wd.selectedEntities.clear();

		if (e != nullptr)
			wd.selectedEntities.push_back(e);

		EditorActionEntitySelection::Create(m_editor, wd.world->GetID(), prev, wd.selectedEntities);

		BroadcastEntitySelectionChanged(wd.world, wd.selectedEntities);
	}

	void EditorWorldManager::AddResourcesToWorld(EntityWorld* world, const Vector<ResourceDirectory*>& dirs, const Vector3& pos)
	{
		ResourceManagerV2& rm = m_editor->GetApp()->GetResourceManager();

		Vector<Entity*> entities;
		entities.reserve(dirs.size());
		for (ResourceDirectory* dir : dirs)
		{
			if (dir->resourceTID == GetTypeID<Model>())
			{
				WorldUtility::LoadModelAndMaterials(m_editor, dir->resourceID, world->GetID());

				Model*	model  = rm.GetResource<Model>(dir->resourceID);
				Entity* entity = WorldUtility::AddModelToWorld(world, model, model->GetMeta().materials);
				world->LoadMissingResources(rm, m_editor->GetProjectManager().GetProjectData(), {}, world->GetID());
				entity->SetPosition(pos);
				entities.push_back(entity);
			}
		}

		EditorActionEntitiesCreated::Create(m_editor, world, entities);
	}

	EditorWorldRenderer* EditorWorldManager::OpenWorld(ResourceID id)
	{
		WorldData worldData = {};

		EntityWorld*		 world				 = new EntityWorld(id, "");
		WorldRenderer*		 worldRenderer		 = new WorldRenderer(&m_editor->GetApp()->GetGfxContext(), &m_editor->GetApp()->GetResourceManager(), world, Vector2ui(4, 4), "WorldRenderer: " + world->GetName());
		EditorWorldRenderer* editorWorldRenderer = new EditorWorldRenderer(m_editor, m_editor->GetApp()->GetLGX(), worldRenderer);

		if (id != 0)
		{
			const String resourcePath = m_editor->GetProjectManager().GetProjectData()->GetResourcePath(id);

			if (!FileSystem::FileOrPathExists(resourcePath))
			{
				LINA_ERR("Can not open world with id {0}", id);
				delete world;
				delete worldRenderer;
				delete editorWorldRenderer;
				return nullptr;
			}

			IStream stream = Serialization::LoadFromFile(resourcePath.c_str());
			world->LoadFromStream(stream);
			stream.Destroy();

			HashSet<ResourceID> defaultResources = {

			};
			world->LoadMissingResources(m_editor->GetApp()->GetResourceManager(), m_editor->GetProjectManager().GetProjectData(), defaultResources, world->GetID());
		}

		m_editor->GetApp()->JoinRender();
		m_editor->GetEditorRenderer().AddWorldRenderer(worldRenderer, editorWorldRenderer);

		m_worlds.push_back({
			.world				 = world,
			.worldRenderer		 = worldRenderer,
			.editorWorldRenderer = editorWorldRenderer,
		});

		for (EditorWorldManagerListener* l : m_listeners)
			l->OnWorldManagerOpenedWorld(editorWorldRenderer);

		return editorWorldRenderer;
	}

	void EditorWorldManager::CloseWorld(EntityWorld* world)
	{
		auto it = linatl::find_if(m_worlds.begin(), m_worlds.end(), [world](const WorldData& wd) -> bool { return wd.world == world; });

		if (it == m_worlds.end())
		{
			LINA_ERR("Could not find world to close!");
			return;
		}

		m_editor->GetApp()->JoinRender();

		WorldData& data = *it;

		for (EditorWorldManagerListener* l : m_listeners)
			l->OnWorldManagerClosingWorld(data.editorWorldRenderer);

		m_editor->GetEditorRenderer().RemoveWorldRenderer(data.worldRenderer);
		delete data.worldRenderer;
		delete data.editorWorldRenderer;
		delete data.world;

		m_worlds.erase(it);
	}

	void EditorWorldManager::OnActionDeletingEntities(ResourceID world, const Vector<EntityID>& entities)
	{
		auto it = linatl::find_if(m_worlds.begin(), m_worlds.end(), [world](const WorldData& wd) -> bool { return wd.world->GetID() == world; });
		if (it == m_worlds.end())
			return;
		WorldData& wd = *it;

		Vector<Entity*> newSelection;
		for (EntityID e : entities)
		{
			auto contains = linatl::find_if(wd.selectedEntities.begin(), wd.selectedEntities.end(), [e](Entity* ent) -> bool { return ent->GetGUID() == e; });
		}
	}

	void EditorWorldManager::OnActionEntitySelection(ResourceID world, const Vector<EntityID>& entities)
	{
		auto it = linatl::find_if(m_worlds.begin(), m_worlds.end(), [world](const WorldData& wd) -> bool { return wd.world->GetID() == world; });
		if (it == m_worlds.end())
			return;
		WorldData& wd = *it;

		wd.selectedEntities.resize(0);

		for (EntityID id : entities)
		{
			Entity* e = wd.world->GetEntity(id);
			if (e == nullptr)
				continue;
			wd.selectedEntities.push_back(e);
		}

		BroadcastEntitySelectionChanged(wd.world, wd.selectedEntities);
	}

	void EditorWorldManager::OnActionEntityTransform(ResourceID world, const Vector<EntityID>& entities, const Vector<Transformation>& transforms)
	{
		auto it = linatl::find_if(m_worlds.begin(), m_worlds.end(), [world](const WorldData& wd) -> bool { return wd.world->GetID() == world; });
		if (it == m_worlds.end())
			return;
		WorldData& wd = *it;

		size_t i = 0;
		for (EntityID id : entities)
		{
			Entity* e = wd.world->GetEntity(id);
			if (e != nullptr)
				e->SetTransform(transforms.at(i));
			i++;
		}
	}

	EntityWorld* EditorWorldManager::GetWorld(ResourceID id)
	{
		auto it = linatl::find_if(m_worlds.begin(), m_worlds.end(), [id](const WorldData& wd) -> bool { return wd.world->GetID() == id; });
		if (it == m_worlds.end())
			return nullptr;
		return it->world;
	}

	void EditorWorldManager::BroadcastEntitySelectionChanged(EntityWorld* world, const Vector<Entity*>& selection)
	{
		for (EditorWorldManagerListener* l : m_listeners)
			l->OnWorldManagerEntitySelectionChanged(world, selection);
	}

	EditorWorldManager::WorldData& EditorWorldManager::GetWorldData(EntityWorld* world)
	{
		auto it = linatl::find_if(m_worlds.begin(), m_worlds.end(), [world](const WorldData& wd) -> bool { return wd.world == world; });
		LINA_ASSERT(it != m_worlds.end(), "");
		return *it;
	}

} // namespace Lina::Editor
