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

#include "Core/World/EntityWorld.hpp"
#include "Core/World/Entity.hpp"
#include "Core/World/Component.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Data/ModelNode.hpp"
#include "Core/Audio/Audio.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Components/MeshComponent.hpp"
#include "Core/Components/CameraComponent.hpp"

#include "Common/System/SystemInfo.hpp"

#include "Common/Serialization/Serialization.hpp"
#include "Core/Application.hpp"

namespace Lina
{
#define ENTITY_VEC_SIZE_CHUNK 2000

	EntityWorld::EntityWorld(ResourceID id, const String& name) : Resource(id, GetTypeID<EntityWorld>(), name), m_worldInput(&Application::GetLGX()->GetInput(), &m_screen), m_physicsWorld(this){};

	EntityWorld::~EntityWorld()
	{
		m_resourceManagerV2.Shutdown();

		m_entityBucket.View([this](Entity* e, uint32 index) -> bool {
			if (e->GetParent() == nullptr)
				DestroyEntity(e);
			return false;
		});

		for (Pair<TypeID, ComponentCacheBase*> pair : m_componentCaches)
		{
			delete pair.second;
		}
	}

	Entity* EntityWorld::CreateEntity(const String& name)
	{
		const uint32 id = m_entityBucket.GetActiveItemCount();
		Entity*		 e	= m_entityBucket.Allocate();
		e->m_guid		= ConsumeEntityGUID();
		e->m_world		= this;
		e->m_name		= name;
		return e;
	}

	void EntityWorld::DestroyEntity(Entity* e)
	{
		if (e->m_parent != nullptr)
			e->m_parent->RemoveChild(e);
		DestroyEntityData(e);

		for (auto& [tid, cache] : m_componentCaches)
		{
			cache->Destroy(e);
		}
	}

	void EntityWorld::DestroyEntityData(Entity* e)
	{
		for (auto child : e->m_children)
			DestroyEntityData(child);
		m_entityBucket.Free(e);
	}

	void EntityWorld::SaveToStream(OStream& stream) const
	{
		Resource::SaveToStream(stream);
		stream << VERSION;
		stream << m_entityGUIDCounter;
		stream << m_gfxSettings;

		const uint32 entitiesSize = m_entityBucket.GetActiveItemCount();
		stream << entitiesSize;

		m_entityBucket.View([&](Entity* e, uint32 index) -> bool {
			e->SaveToStream(stream);
			return false;
		});

		m_entityBucket.View([&](Entity* e, uint32 index) -> bool {
			if (e->GetParent() != nullptr)
				stream << e->m_guid;
			else
				stream << 0;
			return false;
		});

		const uint32 cacheSize = static_cast<uint32>(m_componentCaches.size());
		stream << cacheSize;

		for (auto& [tid, cache] : m_componentCaches)
		{
			stream << tid;
			cache->SaveToStream(stream);
		}
	}

	bool EntityWorld::LoadFromFile(const String& path)
	{
		IStream stream = Serialization::LoadFromFile(path.c_str());
		if (!stream.Empty())
			LoadFromStream(stream);
		else
			return false;

		stream.Destroy();
		return true;
	}

	void EntityWorld::LoadFromStream(IStream& stream)
	{
		Resource::LoadFromStream(stream);
		uint32 version = 0;
		stream >> version;
		stream >> m_entityGUIDCounter;
		stream >> m_gfxSettings;

		m_entityBucket.Clear();

		uint32 entitiesSize = 0;
		stream >> entitiesSize;
		Vector<Entity*> tempEntities;
		tempEntities.resize(static_cast<size_t>(entitiesSize));

		for (uint32 i = 0; i < entitiesSize; i++)
		{
			Entity* e = CreateEntity("");
			e->LoadFromStream(stream);
			tempEntities[i] = e;
		}

		for (uint32 i = 0; i < entitiesSize; i++)
		{
			EntityID parentGUID = 0;
			stream >> parentGUID;

			if (parentGUID != 0)
			{
				Entity* parent = m_entityBucket.Find([parentGUID](Entity* e) -> bool { return e->m_guid == parentGUID; });
				if (parent != nullptr)
					parent->AddChild(tempEntities[i]);
			}
		}

		uint32 cachesSize = 0;
		stream >> cachesSize;

		for (uint32 i = 0; i < cachesSize; i++)
		{
			TypeID tid = 0;
			stream >> tid;

			MetaType&			type  = ReflectionSystem::Get().Resolve(tid);
			void*				ptr	  = type.GetFunction<void*(EntityWorld*)>("CreateComponentCache"_hs)(this);
			ComponentCacheBase* cache = static_cast<ComponentCacheBase*>(ptr);
			cache->LoadFromStream(stream, tempEntities);
			m_componentCaches[tid] = cache;

			cache->ForEach([this](Component* c) { OnCreateComponent(c, c->m_entity); });
		}
	}

	void EntityWorld::AddListener(EntityWorldListener* listener)
	{
		m_listeners.push_back(listener);
	}

	void EntityWorld::RemoveListener(EntityWorldListener* listener)
	{
		m_listeners.erase(linatl::find_if(m_listeners.begin(), m_listeners.end(), [listener](EntityWorldListener* list) -> bool { return list == listener; }));
	}

	void EntityWorld::OnCreateComponent(Component* c, Entity* e)
	{
		c->m_world	= this;
		c->m_entity = e;
		c->OnEvent({.type = ComponentEventType::Create});
		for (auto* l : m_listeners)
			l->OnComponentAdded(c);
	}

	void EntityWorld::OnDestroyComponent(Component* c, Entity* e)
	{
		if (c == m_activeCamera)
			m_activeCamera = nullptr;

		c->OnEvent({.type = ComponentEventType::Destroy});
		for (auto* l : m_listeners)
			l->OnComponentRemoved(c);
	}

	namespace
	{
		void ProcessModelNode(Model* model, EntityWorld* world, Entity* parent, ModelNode* node, const Vector<ResourceID>& materials)
		{
			Entity* nodeEntity = world->CreateEntity(node->GetName());
			parent->AddChild(nodeEntity);
			nodeEntity->SetLocalTransformation(node->GetLocalMatrix());

			const Vector3 nodeEntityPos = nodeEntity->GetPosition();
			MeshDefault*  mesh			= node->GetMesh();

			if (mesh != nullptr)
			{
				const uint32   matIdx = model->GetMesh(node->GetMeshIndex())->GetPrimitives().front().GetMaterialIndex();
				MeshComponent* mc	  = world->AddComponent<MeshComponent>(nodeEntity);
				mc->SetMesh(model->GetID(), node->GetMeshIndex());

				if (materials.size() > matIdx)
					mc->SetMaterial(materials[matIdx]);
			}

			for (ModelNode* child : node->GetChildren())
				ProcessModelNode(model, world, nodeEntity, child, materials);
		}
	} // namespace

	Entity* EntityWorld::AddModelToWorld(Model* model, const Vector<ResourceID>& materials)
	{
		LINA_ASSERT(materials.size() >= model->GetMeta().materials.size(), "");

		Entity*					 base  = CreateEntity(model->GetName());
		const Vector<ModelNode*> roots = model->GetRootNodes();
		for (ModelNode* rootNode : roots)
			ProcessModelNode(model, this, base, rootNode, materials);
		return base;
	}

	void EntityWorld::CollectResourceNeeds(HashSet<ResourceID>& outResources)
	{
		for (auto [tid, cache] : m_componentCaches)
		{
			cache->ForEach([&](Component* c) { c->CollectReferences(outResources); });
		}

		outResources.insert(m_gfxSettings.lightingMaterial);
		outResources.insert(m_gfxSettings.skyMaterial);
		outResources.insert(m_gfxSettings.skyModel);
	}

	void EntityWorld::LoadMissingResources(ProjectData* project, const HashSet<ResourceID>& extraResources)
	{
		HashSet<ResourceID> resources = extraResources;
		CollectResourceNeeds(resources);

		// First load whatever is requested + needed by the components.
		if (!resources.empty())
			m_resourceManagerV2.LoadResourcesFromProject(project, resources, [](uint32 loaded, Resource* current) {});

		// Load materials required by the models.
		HashSet<ResourceID>	  modelMaterials;
		ResourceCache<Model>* modelCache = m_resourceManagerV2.GetCache<Model>();
		modelCache->View([&modelMaterials](Model* model, uint32 index) -> bool {
			for (ResourceID mat : model->GetMeta().materials)
				modelMaterials.insert(mat);

			return false;
		});
		m_resourceManagerV2.LoadResourcesFromProject(project, modelMaterials, [](uint32 loaded, Resource* current) {});

		// Load shaders, textures and samplers required by the materials.
		HashSet<ResourceID>		 materialDependencies;
		ResourceCache<Material>* materialCache = m_resourceManagerV2.GetCache<Material>();
		materialCache->View([&materialDependencies](Material* mat, uint32 index) -> bool {
			materialDependencies.insert(mat->GetShader());
			const Vector<MaterialProperty*> props = mat->GetProperties();
			for (MaterialProperty* prop : props)
			{
				if (prop->propDef.type == ShaderPropertyType::Texture2D)
				{
					LinaTexture2D* txt = reinterpret_cast<LinaTexture2D*>(prop->data.data());
					materialDependencies.insert(txt->texture);
					materialDependencies.insert(txt->sampler);
				}
			}
			return false;
		});
	}
} // namespace Lina
