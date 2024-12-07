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

#include "Editor/Widgets/Panel/PanelModelViewer.hpp"
#include "Editor/Widgets/World/WorldDisplayer.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Editor/Graphics/GridRenderer.hpp"
#include "Editor/Editor.hpp"
#include "Core/Application.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"

namespace Lina::Editor
{
	void PanelModelViewer::Construct()
	{
		PanelResourceViewer::Construct();

		m_worldDisplayer = m_manager->Allocate<WorldDisplayer>("WorldDisplayer");
		m_worldDisplayer->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_worldDisplayer->SetAlignedPos(Vector2::Zero);
		m_worldDisplayer->SetAlignedSize(Vector2::One);

		m_resourceBG->AddChild(m_worldDisplayer);
	}

	void PanelModelViewer::Initialize()
	{
		PanelResourceViewer::Initialize();

		if (!m_resource)
			return;

		if (m_world)
			return;

		m_world			= new EntityWorld(0, "");
		m_worldRenderer = new WorldRenderer(&m_editor->GetApp()->GetGfxContext(), &m_editor->GetApp()->GetResourceManager(), m_world, Vector2ui(4, 4), "WorldRenderer: " + m_resource->GetName() + " :");
		m_gridRenderer	= new GridRenderer(m_editor, m_editor->GetApp()->GetLGX(), m_world, &m_editor->GetApp()->GetResourceManager());
		m_worldRenderer->AddFeatureRenderer(m_gridRenderer);

		m_editor->GetApp()->JoinRender();
		m_editor->GetApp()->GetWorldProcessor().AddWorld(m_world);
		m_editor->GetEditorRenderer().AddWorldRenderer(m_worldRenderer);

		m_worldDisplayer->DisplayWorld(m_worldRenderer);
		m_worldDisplayer->CreateOrbitCamera();

		m_world->GetGfxSettings().lightingMaterial = EDITOR_MATERIAL_DEFAULT_LIGHTING_ID;
		m_world->GetGfxSettings().skyModel		   = EDITOR_MODEL_SKYSPHERE_ID;
		m_world->GetGfxSettings().skyMaterial	   = EDITOR_MATERIAL_DEFAULT_SKY_ID;

		HashSet<ResourceID> initialResources = {
			EDITOR_MODEL_CUBE_ID,
			EDITOR_MODEL_SPHERE_ID,
			EDITOR_MODEL_PLANE_ID,
			EDITOR_MODEL_CYLINDER_ID,
			EDITOR_MODEL_CAPSULE_ID,
			EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID,
			m_resource->GetID(),
		};

		Model* model = static_cast<Model*>(m_resource);
		for (ResourceID id : model->GetMeta().materials)
			initialResources.insert(id);

		m_world->LoadMissingResources(m_editor->GetApp()->GetResourceManager(), m_editor->GetProjectManager().GetProjectData(), initialResources, m_resourceSpace);
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();

		SetupWorld();

		UpdateResourceProperties();
		RebuildContents();
	}

	void PanelModelViewer::Destruct()
	{
		PanelResourceViewer::Destruct();

		if (m_world)
		{
			m_editor->GetApp()->JoinRender();
			m_editor->GetApp()->GetWorldProcessor().RemoveWorld(m_world);
			m_editor->GetEditorRenderer().RemoveWorldRenderer(m_worldRenderer);
			delete m_gridRenderer;
			delete m_worldRenderer;
			delete m_world;
			m_worldRenderer = nullptr;
			m_world			= nullptr;
		}
	}

	void PanelModelViewer::StoreEditorActionBuffer()
	{
	}

	void PanelModelViewer::UpdateResourceProperties()
	{
		Model* model   = static_cast<Model*>(m_resource);
		m_modelName	   = model->GetName();
		m_animations   = "0";
		m_materialDefs = TO_STRING(model->GetMaterialDefs().size());
		m_meshes	   = TO_STRING(model->GetMeshes().size());
	}

	void PanelModelViewer::RebuildContents()
	{
		m_inspector->DeallocAllChildren();
		m_inspector->RemoveAllChildren();
		CommonWidgets::BuildClassReflection(m_inspector, this, ReflectionSystem::Get().Resolve<PanelModelViewer>(), [this](MetaType* meta, FieldBase* field) {
			// SetupWorld();
		});

		m_inspector->Initialize();

		if (m_previewOnly)
			DisableRecursively(m_inspector);
	}

	void PanelModelViewer::SetupWorld()
	{
		ResourceManagerV2& rm = m_editor->GetApp()->GetResourceManager();

		Resource* res = rm.GetIfExists(m_resource->GetTID(), m_resource->GetID());

		if (res == nullptr)
			return;

		if (m_displayEntity != nullptr)
			m_world->DestroyEntity(m_displayEntity);

		Material* defaultSky = rm.GetResource<Material>(EDITOR_MATERIAL_DEFAULT_SKY_ID);
		SetupDefaultSkyMaterial(defaultSky);

		Model* model	= static_cast<Model*>(m_resource);
		m_displayEntity = m_world->AddModelToWorld(model, model->GetMeta().materials);
	}

} // namespace Lina::Editor
