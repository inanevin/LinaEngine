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
#include "Core/Audio/Audio.hpp"
#include "Core/Physics/PhysicsMaterial.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Data/ModelNode.hpp"
#include "Core/World/Components/CompModel.hpp"
#include "Core/World/WorldUtility.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Resources/ResourceCache.hpp"

#include "Common/System/SystemInfo.hpp"

#include "Common/Serialization/Serialization.hpp"
#include "Core/Application.hpp"
#include "Core/Physics/PhysicsWorld.hpp"
#include "Core/World/EntityTemplate.hpp"

namespace Lina
{
#define ENTITY_VEC_SIZE_CHUNK 2000

	EntityWorld::EntityWorld(ResourceID id, const String& name) : Resource(id, GetTypeID<EntityWorld>(), name), m_worldInput(&Application::GetLGX()->GetInput(), &m_screen, this)
	{
		m_physicsWorld = new PhysicsWorld(this);
	};

	EntityWorld::~EntityWorld()
	{
		m_entityBucket.View([this](Entity* e, uint32 index) -> bool {
			if (e->GetParent() == nullptr)
				DestroyEntity(e);
			return false;
		});

		DestroyComponentCaches();
		if (m_physicsWorld)
			delete m_physicsWorld;
		m_physicsWorld = nullptr;
	}

	void EntityWorld::Initialize(ResourceManagerV2* rm, LinaGX::Window* window, ProjectData* projectData)
	{
		m_projectData = projectData;
		m_rm		  = rm;
		m_screen.SetOwnerWindow(window);

		m_entityBucket.View([this, rm](Entity* e, uint32 idx) -> bool {
			Vector<Component*> comps;
			GetComponents(e, comps);
			for (Component* c : comps)
				c->m_resourceManager = rm;

			return false;
		});
	}

	void EntityWorld::Tick(float delta)
	{
		if (m_playMode == PlayMode::Play)
		{
			m_physicsWorld->Simulate(delta);
		}
		else if (m_playMode == PlayMode::Physics)
		{
			m_physicsWorld->Simulate(delta);
		}

		for (EntityWorldListener* l : m_listeners)
			l->OnWorldTick(delta, m_playMode);

		GetCache<CompModel>()->GetBucket().View([delta](CompModel* comp, uint32 idx) -> bool {
			comp->GetAnimationController().TickAnimation(delta);
			return false;
		});
	}

	void EntityWorld::SetPlayMode(PlayMode playmode)
	{
		if (m_playMode != PlayMode::Physics && playmode == PlayMode::Physics)
			m_physicsWorld->Begin();

		if (m_playMode == PlayMode::Physics && playmode == PlayMode::None)
			m_physicsWorld->End();

		if (m_playMode != PlayMode::Play && playmode == PlayMode::Play)
			BeginPlay();

		if (m_playMode == PlayMode::Play && playmode != PlayMode::Play)
			EndPlay();

		m_playMode = playmode;
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
		DestroyComponentCaches();

		Resource::LoadFromStream(stream);
		uint32 version = 0;
		stream >> version;
		stream >> m_entityGUIDCounter;
		stream >> m_gfxSettings;
		m_entityBucket.Clear();

		Vector<Entity*> entities;
		WorldUtility::LoadEntitiesFromStream(stream, this, entities);

		stream >> m_neededResourceDefinitions;
	}

	void EntityWorld::SaveToStream(OStream& stream) const
	{
		Resource::SaveToStream(stream);
		stream << VERSION;
		stream << m_entityGUIDCounter;
		stream << m_gfxSettings;

		const uint32	entitiesSize = m_entityBucket.GetActiveItemCount();
		Vector<Entity*> entities;
		entities.reserve(entitiesSize);
		m_entityBucket.View([&](Entity* e, uint32 index) -> bool {
			entities.push_back(e);
			return false;
		});

		Vector<Entity*> roots;
		roots.reserve(entities.size());
		WorldUtility::ExtractRoots(entities, roots);

		WorldUtility::SaveEntitiesToStream(stream, this, roots);

		// Collect maybe.
		stream << m_neededResourceDefinitions;
	}

	void EntityWorld::CollectResourceNeeds(HashSet<ResourceID>& outResources) const
	{
		outResources.insert(ENGINE_MODEL_CUBE_ID);
		outResources.insert(ENGINE_MODEL_SPHERE_ID);
		outResources.insert(ENGINE_MODEL_CYLINDER_ID);
		outResources.insert(ENGINE_MODEL_CAPSULE_ID);
		outResources.insert(ENGINE_MODEL_PLANE_ID);
		outResources.insert(ENGINE_MODEL_QUAD_ID);
		outResources.insert(ENGINE_MODEL_SKYSPHERE_ID);
		outResources.insert(ENGINE_MODEL_SKYCUBE_ID);
		outResources.insert(ENGINE_SHADER_LIGHTING_QUAD_ID);
		outResources.insert(ENGINE_SHADER_WORLD_DEBUG_LINE_ID);
		outResources.insert(ENGINE_SHADER_WORLD_DEBUG_TRIANGLE_ID);
		outResources.insert(ENGINE_SHADER_DEFAULT_SKY_ID);
		outResources.insert(ENGINE_SHADER_DEFAULT_OPAQUE_SURFACE_ID);
		outResources.insert(ENGINE_SHADER_DEFAULT_TRANSPARENT_SURFACE_ID);
		outResources.insert(ENGINE_SHADER_GUI_DEFAULT_ID);
		outResources.insert(ENGINE_SHADER_GUI_TEXT_ID);
		outResources.insert(ENGINE_SHADER_GUI_SDFTEXT_ID);
		outResources.insert(ENGINE_TEXTURE_EMPTY_AO_ID);
		outResources.insert(ENGINE_TEXTURE_EMPTY_ALBEDO_ID);
		outResources.insert(ENGINE_TEXTURE_EMPTY_METALLIC_ROUGHNESS_ID);
		outResources.insert(ENGINE_TEXTURE_EMPTY_NORMAL_ID);
		outResources.insert(ENGINE_TEXTURE_EMPTY_EMISSIVE_ID);
		outResources.insert(ENGINE_SAMPLER_DEFAULT_ID);
		outResources.insert(ENGINE_SAMPLER_GUI_ID);
		outResources.insert(ENGINE_SAMPLER_GUI_TEXT_ID);
		outResources.insert(ENGINE_FONT_ROBOTO_ID);
		outResources.insert(ENGINE_MATERIAL_DEFAULT_SKY_ID);
		outResources.insert(ENGINE_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID);
		outResources.insert(ENGINE_MATERIAL_DEFAULT_TRANSPARENT_OBJECT_ID);
		outResources.insert(ENGINE_MATERIAL_GUI_DEFAULT_ID);
		outResources.insert(ENGINE_MATERIAL_GUI_TEXT_ID);
		outResources.insert(ENGINE_MATERIAL_GUI_SDFTEXT_ID);
		outResources.insert(ENGINE_PHY_MATERIAL_DEFAULT_ID);

		m_entityBucket.View([&](Entity* e, uint32 idx) -> bool {
			outResources.insert(e->GetPhysicsSettings().material);

			const EntityParameters& params = e->GetParams();
			for (const EntityParameter& p : params.params)
			{
				if (p.type == EntityParameterType::ResourceID)
					outResources.insert(p.valRes);
			}
			return false;
		});

		for (const ComponentCachePair& pair : m_componentCaches)
			pair.cache->ForEach([&](Component* c) { c->CollectReferences(outResources); });

		outResources.insert(m_gfxSettings.skyMaterial);
		outResources.insert(m_gfxSettings.skyModel);
	}

	HashSet<Resource*> EntityWorld::LoadMissingResources(ResourceManagerV2& rm, ProjectData* project, const HashSet<ResourceID>& extraResources)
	{
		HashSet<ResourceID> resources = extraResources;
		CollectResourceNeeds(resources);

		for (const ResourceDef& def : m_neededResourceDefinitions)
			resources.insert(def.id);

		HashSet<Resource*> allLoaded;

		// First load whatever is requested + needed by the components.
		if (!resources.empty())
			allLoaded = rm.LoadResourcesFromProject(
				project, resources, [](uint32 loaded, Resource* current) {}, m_id);

		// Widget needs, fonts, textures and materials.
		HashSet<ResourceID>		  widgetRequirements;
		ResourceCache<GUIWidget>* widgetCache = rm.GetCache<GUIWidget>();
		widgetCache->View([&widgetRequirements](GUIWidget* gw, uint32 idx) -> bool {
			const HashSet<ResourceID> refs = gw->GetResourceReferences();
			for (ResourceID id : refs)
				widgetRequirements.insert(id);
			return false;
		});
		HashSet<Resource*> widgetNeeds = rm.LoadResourcesFromProject(project, widgetRequirements, NULL, m_id);
		allLoaded.insert(widgetNeeds.begin(), widgetNeeds.end());

		// Load materials required by the models.
		HashSet<ResourceID>	  modelMaterials;
		ResourceCache<Model>* modelCache = rm.GetCache<Model>();
		modelCache->View([&modelMaterials](Model* model, uint32 index) -> bool {
			for (ResourceID mat : model->GetMeta().materials)
				modelMaterials.insert(mat);

			return false;
		});
		HashSet<Resource*> modelNeeds = rm.LoadResourcesFromProject(
			project, modelMaterials, [](uint32 loaded, Resource* current) {}, m_id);
		allLoaded.insert(modelNeeds.begin(), modelNeeds.end());

		// Load shaders, textures and samplers required by the materials.
		HashSet<ResourceID>		 materialDependencies;
		ResourceCache<Material>* materialCache = rm.GetCache<Material>();
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
		HashSet<Resource*> matNeeds = rm.LoadResourcesFromProject(
			project, materialDependencies, [](uint32 loaded, Resource* current) {}, m_id);
		allLoaded.insert(matNeeds.begin(), matNeeds.end());

		for (Resource* res : allLoaded)
		{
			const ResourceID id = res->GetID();

			const ResourceDef def = {
				.id	  = id,
				.name = res->GetName(),
				.tid  = res->GetTID(),
			};

			// ->ConsumeResourceID spaces.
			if (id > RESOURCE_ID_CUSTOM_SPACE && id < RESOURCE_ID_ENGINE_SPACE)
				continue;

			m_neededResourceDefinitions.push_back(def);
		}

		RefreshAllComponentReferences();
		return allLoaded;
	}

	Entity* EntityWorld::CreateEntity(EntityID id, const String& name)
	{
		Entity* e  = m_entityBucket.Allocate();
		e->m_guid  = id;
		e->m_world = this;
		e->m_name  = name;
		return e;
	}

	Entity* EntityWorld::GetEntity(EntityID guid)
	{
		Entity* ret = nullptr;
		m_entityBucket.View([&](Entity* e, uint32 idx) -> bool {
			if (e->GetGUID() == guid)
			{
				ret = e;
				return true;
			}

			return false;
		});

		return ret;
	}

	void EntityWorld::DestroyEntity(Entity* e)
	{
		if (e->m_parent != nullptr)
			e->m_parent->RemoveChild(e);
		DestroyEntityData(e);
	}

	Entity* EntityWorld::FindEntity(const String& name)
	{
		const StringID sid	  = TO_SID(name);
		Entity*		   entity = nullptr;

		m_entityBucket.View([&](Entity* e, uint32 idx) -> bool {
			if (TO_SID(e->GetName()) == sid)
			{
				entity = e;
				return true;
			}

			return false;
		});

		return entity;
	}

	Entity* EntityWorld::SpawnTemplate(EntityTemplate* tmp)
	{
		Vector<Entity*> ents = tmp->CreateFromStream(this);
		WorldUtility::FixEntityIDsToNew(this, ents);
		LoadMissingResources(*m_rm, m_projectData, {});

		if (ents.empty())
			return nullptr;

		return ents.at(0);
	}

	void EntityWorld::GetComponents(Entity* e, Vector<Component*>& outComponents) const
	{
		for (const ComponentCachePair& p : m_componentCaches)
		{
			Component* c = GetComponent(e, p.tid);
			if (c != nullptr)
				outComponents.push_back(c);
		}
	}

	Component* EntityWorld::GetComponent(Entity* e, TypeID tid) const
	{
		return GetCache(tid)->Get(e);
	}

	Component* EntityWorld::AddComponent(Entity* e, TypeID tid)
	{
		Component* c = GetCache(tid)->CreateRaw();
		OnCreateComponent(c, e);
		return c;
	}

	void EntityWorld::RefreshComponentReferences(const Vector<Entity*>& entities)
	{
		for (Entity* e : entities)
		{
			const Vector<Entity*>& children = e->GetChildren();
			RefreshComponentReferences(children);

			Vector<Component*> comps;
			GetComponents(e, comps);

			for (Component* c : comps)
				c->StoreReferences();
		}
	}

	void EntityWorld::RefreshAllComponentReferences()
	{
		m_entityBucket.View([this](Entity* e, uint32 idx) -> bool {
			Vector<Component*> comps;
			GetComponents(e, comps);
			for (Component* c : comps)
				c->StoreReferences();

			return false;
		});
	}

	void EntityWorld::AddListener(EntityWorldListener* listener)
	{
		m_listeners.push_back(listener);
	}

	void EntityWorld::RemoveListener(EntityWorldListener* listener)
	{
		m_listeners.erase(linatl::find_if(m_listeners.begin(), m_listeners.end(), [listener](EntityWorldListener* list) -> bool { return list == listener; }));
	}

	void EntityWorld::BeginPlay()
	{
		m_physicsWorld->Begin();

		for (ComponentCachePair& pair : m_componentCaches)
		{
			pair.cache->ForEach([](Component* c) { c->BeginPlay(); });
		}
	}

	void EntityWorld::EndPlay()
	{
		m_physicsWorld->End();
		m_playMode = PlayMode::None;

		for (ComponentCachePair& pair : m_componentCaches)
		{
			pair.cache->ForEach([](Component* c) { c->EndPlay(); });
		}
	}

	void EntityWorld::DestroyEntityData(Entity* e)
	{
		for (auto child : e->m_children)
			DestroyEntityData(child);

		for (const ComponentCachePair& pair : m_componentCaches)
		{
			Component* c = pair.cache->Get(e);

			if (c == nullptr)
				continue;

			OnDestroyComponent(c, e);
			pair.cache->Destroy(c);
		}

		if (e->GetPhysicsBody())
			m_physicsWorld->DestroyBodyForEntity(e);

		m_entityBucket.Free(e);
	}

	void EntityWorld::OnCreateComponent(Component* c, Entity* e)
	{
		c->m_world			 = this;
		c->m_entity			 = e;
		c->m_resourceManager = m_rm;

		if (m_playMode == PlayMode::Play)
			c->BeginPlay();

		for (auto* l : m_listeners)
			l->OnComponentAdded(c);
	}

	void EntityWorld::OnDestroyComponent(Component* c, Entity* e)
	{
		if (m_playMode == PlayMode::Play)
			c->EndPlay();

		for (auto* l : m_listeners)
			l->OnComponentRemoved(c);
	}

	void EntityWorld::DestroyComponentCaches()
	{
		for (const ComponentCachePair& pair : m_componentCaches)
			delete pair.cache;

		m_componentCaches.clear();
	}

	ComponentCacheBase* EntityWorld::GetCache(TypeID tid)
	{
		auto it = linatl::find_if(m_componentCaches.begin(), m_componentCaches.end(), [tid](const ComponentCachePair& pair) -> bool { return pair.tid == tid; });

		if (it == m_componentCaches.end())
		{
			MetaType*			type  = ReflectionSystem::Get().Resolve(tid);
			void*				ptr	  = type->GetFunction<void*(EntityWorld*)>("CreateComponentCache"_hs)(this);
			ComponentCacheBase* cache = static_cast<ComponentCacheBase*>(ptr);
			m_componentCaches.push_back({
				.tid   = tid,
				.cache = cache,
			});
			return cache;
		}

		return it->cache;
	}

	ComponentCacheBase* EntityWorld::GetCache(TypeID tid) const
	{
		auto it = linatl::find_if(m_componentCaches.begin(), m_componentCaches.end(), [tid](const ComponentCachePair& pair) -> bool { return pair.tid == tid; });
		LINA_ASSERT(it != m_componentCaches.end(), "");
		return it->cache;
	}

} // namespace Lina
