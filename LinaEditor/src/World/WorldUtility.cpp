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

#include "Editor/World/WorldUtility.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Editor.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Application.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/World/Components/CompModel.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Common/Serialization/Serialization.hpp"

namespace Lina::Editor
{
	void WorldUtility::SetupDefaultSkyMaterial(Material* mat, ResourceManagerV2* rm)
	{
		if (rm)
			mat->SetShader(rm->GetResource<Shader>(EDITOR_SHADER_DEFAULT_SKY_ID));
		mat->SetProperty("skyColor"_hs, Vector3(0.38f, 0.521f, 0.807f));
		mat->SetProperty("horizonColor"_hs, Vector3(0.386f, 0.521f, 0.807f));
		mat->SetProperty("groundColor"_hs, Vector3(0.011f, 0.021f, 0.05f));
		mat->SetProperty("sunPosition"_hs, Vector2(0.0f, 0.0f));
		mat->SetProperty("sunColor"_hs, Vector3(1.0f, 1.0f, 1.0f));
		mat->SetProperty("sunSize"_hs, 0.0f);
		mat->SetProperty("sunDiffusion"_hs, 5.0f);
		mat->SetProperty("horizonPosition"_hs, 0.0f);
		mat->SetProperty("horizonDiffusion"_hs, 0.15f);
	}

	Entity* WorldUtility::AddModelToWorld(EntityID guid, EntityWorld* world, Model* model, const Vector<ResourceID>& materials)
	{
		LINA_ASSERT(materials.size() >= model->GetMeta().materials.size(), "");

		Entity* base = world->CreateEntity(guid, model->GetName());

		CompModel* modelComp = world->AddComponent<CompModel>(base);

		uint32 idx = 0;

		for (ResourceID material : materials)
		{
			modelComp->SetMaterial(material, idx++);
		}

		const uint32 modelMatsSize = static_cast<uint32>(model->GetMeta().materials.size());
		if (idx < modelMatsSize)
		{
			for (uint32 i = 0; i < modelMatsSize - idx; i++)
				modelComp->SetMaterial(materials.at(0), idx + i);
		}

		modelComp->SetModel(model);

		return base;
	}

	void WorldUtility::LoadModelAndMaterials(Editor* editor, ResourceID model, ResourceID resourceSpace)
	{
		const String& modelPath = editor->GetProjectManager().GetProjectData()->GetResourcePath(model);

		IStream stream = Serialization::LoadFromFile(modelPath.c_str());
		Model	mdl(0, "");
		mdl.LoadFromStream(stream);
		stream.Destroy();

		HashSet<ResourceID> resources;
		resources.insert(model);

		for (ResourceID mat : mdl.GetMeta().materials)
			resources.insert(mat);

		editor->GetApp()->GetResourceManager().LoadResourcesFromProject(editor->GetProjectManager().GetProjectData(), resources, {}, resourceSpace);
	}

	Vector<Entity*> WorldUtility::AddResourcesToWorld(Editor* editor, EntityWorld* world, const Vector<ResourceDirectory*>& dirs, const Vector3& pos)
	{
		ResourceManagerV2& rm = editor->GetApp()->GetResourceManager();

		Entity*			last = nullptr;
		Vector<Entity*> entities;
		entities.reserve(dirs.size());
		for (ResourceDirectory* dir : dirs)
		{
			if (dir->resourceTID == GetTypeID<Model>())
			{
				WorldUtility::LoadModelAndMaterials(editor, dir->resourceID, world->GetID());

				Model*	model  = rm.GetResource<Model>(dir->resourceID);
				Entity* entity = WorldUtility::AddModelToWorld(world->ConsumeEntityGUID(), world, model, model->GetMeta().materials);
				world->LoadMissingResources(rm, editor->GetProjectManager().GetProjectData(), {}, world->GetID());
				entity->SetPosition(pos);
				entities.push_back(entity);
			}
		}

		return entities;
	}

	void WorldUtility::FixEntityIDsToNew(EntityWorld* world, const Vector<Entity*>& entities)
	{
		for (Entity* e : entities)
		{
			e->SetGUID(world->ConsumeEntityGUID());

			const Vector<Entity*>& children = e->GetChildren();
			FixEntityIDsToNew(world, children);

			Vector<EntityID>& childIds = e->GetChildrenIDs();
			childIds.resize(0);

			for (Entity* c : children)
				childIds.push_back(c->GetGUID());
		}
	}

	void WorldUtility::SaveEntitiesToStream(OStream& stream, EntityWorld* world, const Vector<Entity*>& entities)
	{
		const uint32 sz = static_cast<uint32>(entities.size());
		stream << sz;

		Vector<Component*> comps;
		for (Entity* e : entities)
		{
			e->SaveToStream(stream);

			comps.resize(0);
			world->GetComponents(e, comps);

			const uint32 csz = static_cast<uint32>(comps.size());
			stream << csz;

			for (Component* c : comps)
			{
				stream << c->GetTID();
				c->SaveToStream(stream);
			}
		}

		for (Entity* e : entities)
		{
			const Vector<Entity*>& children = e->GetChildren();
			const uint32		   sz		= static_cast<uint32>(children.size());
			stream << sz;

			if (sz != 0)
				SaveEntitiesToStream(stream, world, children);
		}
	}
	void WorldUtility::LoadEntitiesFromStream(IStream& stream, EntityWorld* world, Vector<Entity*>& outEntities)
	{
		uint32 sz = 0;
		stream >> sz;

		for (uint32 i = 0; i < sz; i++)
		{
			Entity* e = world->CreateEntity(0, "");
			e->LoadFromStream(stream);
			e->SetTransform(e->GetTransform());

			uint32 csz = 0;
			stream >> csz;

			for (size_t j = 0; j < csz; j++)
			{
				TypeID tid = 0;
				stream >> tid;
				Component* c = world->AddComponent(e, tid);
				c->LoadFromStream(stream);
				c->StoreReferences();
			}

			outEntities.push_back(e);
		}

		for (Entity* e : outEntities)
		{
			uint32 childSz = 0;
			stream >> childSz;

			Vector<Entity*> children;
			if (childSz != 0)
				LoadEntitiesFromStream(stream, world, children);

			for (Entity* c : children)
				e->AddChild(c);
		}
	}

	void WorldUtility::DuplicateEntities(Editor* editor, EntityWorld* world, const Vector<Entity*>& srcEntities, Vector<Entity*>& outEntities)
	{
		OStream stream;
		SaveEntitiesToStream(stream, world, srcEntities);

		IStream inStream;
		inStream.Create(stream.GetDataRaw(), stream.GetCurrentSize());
		LoadEntitiesFromStream(inStream, world, outEntities);

		FixEntityIDsToNew(world, outEntities);

		const Vector3 camRight = world->GetWorldCamera().GetRotation().GetRight();
		size_t		  i		   = 0;
		for (Entity* e : outEntities)
		{
			const Vector3 er  = srcEntities.at(i)->GetRotation().GetRight();
			const float	  dot = er.Dot(camRight);
			e->SetPosition(srcEntities.at(i)->GetPosition() + er * 0.5f * (dot > 0.0f ? 1.0f : -1.0f)); // todo: aabb this later
			i++;
		}
	}
} // namespace Lina::Editor
