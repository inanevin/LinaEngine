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

#include "Core/World/WorldUtility.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Application.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/World/Components/CompModel.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Common/Serialization/Serialization.hpp"

namespace Lina
{
	void WorldUtility::FixEntityIDsToNew(EntityWorld* world, const Vector<Entity*>& entities)
	{
		for (Entity* e : entities)
		{
			e->SetGUID(world->ConsumeEntityGUID());
			const Vector<Entity*>& children = e->GetChildren();
			FixEntityIDsToNew(world, children);
		}
	}

	void WorldUtility::SaveEntitiesToStream(OStream& stream, const EntityWorld* world, const Vector<Entity*>& entities, Entity* parent)
	{
		const uint32 sz = static_cast<uint32>(entities.size());
		stream << sz;

		Vector<Component*> comps;
		for (Entity* e : entities)
		{
			e->SaveToStream(stream);

			const EntityID parentID = parent == nullptr ? (e->GetParent() ? e->GetParent()->GetGUID() : 0) : 0;
			stream << parentID;

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
				SaveEntitiesToStream(stream, world, children, e);
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

			EntityID parent = 0;
			stream >> parent;

			if (parent != 0)
				world->GetEntity(parent)->AddChild(e);

			uint32 csz = 0;
			stream >> csz;

			for (size_t j = 0; j < csz; j++)
			{
				TypeID tid = 0;
				stream >> tid;
				Component* c = world->AddComponent(e, tid);
				c->LoadFromStream(stream);
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

	void WorldUtility::DuplicateEntities(EntityWorld* world, const Vector<Entity*>& srcEntities, Vector<Entity*>& outEntities)
	{
		Vector<Entity*> roots;
		ExtractRoots(srcEntities, roots);

		OStream stream;
		SaveEntitiesToStream(stream, world, roots);

		IStream inStream;
		inStream.Create(stream.GetDataRaw(), stream.GetCurrentSize());
		LoadEntitiesFromStream(inStream, world, outEntities);
		world->RefreshComponentReferences(outEntities);

		FixEntityIDsToNew(world, outEntities);

		const Vector3 camRight = world->GetWorldCamera().GetRotation().GetRight();
		size_t		  i		   = 0;
		for (Entity* e : outEntities)
		{
			const Vector3 er  = roots.at(i)->GetRotation().GetRight();
			const float	  dot = er.Dot(camRight);
			e->SetPosition(roots.at(i)->GetPosition() + er * 0.5f * (dot > 0.0f ? 1.0f : -1.0f)); // todo: aabb this later
			i++;
		}
	}

	void WorldUtility::ExtractRoots(const Vector<Entity*>& entities, Vector<Entity*>& roots)
	{
		roots = entities;
		HashSet<Entity*> toRemove;
		for (Entity* e : entities)
		{
			for (Entity* s : entities)
			{
				if (s == e)
					continue;

				if (e->FindInChildHierarchy(s->GetGUID()))
					toRemove.insert(s);
			}
		}

		for (Entity* e : toRemove)
			roots.erase(linatl::find_if(roots.begin(), roots.end(), [e](Entity* ent) -> bool { return ent == e; }));
	}
} // namespace Lina
