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

#include "Editor/Widgets/Panel/PanelPhysicsMaterialViewer.hpp"
#include "Editor/Widgets/World/WorldDisplayer.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Graphics/EditorWorldRenderer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Actions/EditorActionResources.hpp"
#include "Editor/World/EditorWorldUtility.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Application.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina::Editor
{

	void PanelPhysicsMaterialViewer::Construct()
	{
		PanelResourceViewer::Construct();

		m_resourceBG->GetWidgetProps().childMargins = TBLR::Eq(0.0f);
		WorldDisplayer* displayer					= m_manager->Allocate<WorldDisplayer>("WorldDisplayer");
		displayer->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		displayer->SetAlignedPos(Vector2::Zero);
		displayer->SetAlignedSize(Vector2::One);
		m_resourceBG->AddChild(displayer);
		m_worldDisplayer = displayer;

		displayer->GetProps().noWorldText = Locale::GetStr(LocaleStr::ResourceNotFound);
	}

	void PanelPhysicsMaterialViewer::Destruct()
	{
		PanelResourceViewer::Destruct();

		m_previousStream.Destroy();

		if (m_world)
			m_editor->GetWorldManager().DestroyEditorWorld(m_world);
		m_world = nullptr;
	}

	void PanelPhysicsMaterialViewer::Initialize()
	{
		PanelResourceViewer::Initialize();
		if (!m_resource)
			return;

		if (m_world)
			return;

		EditorWorldRenderer* ewr = m_editor->GetWorldManager().CreateEditorWorld();
		m_world					 = ewr->GetWorldRenderer()->GetWorld();
		m_world->SetID(m_resourceSpace);
		m_worldDisplayer->DisplayWorld(ewr, WorldCameraType::Orbit);

		// Resource set up.
		m_world->GetGfxSettings().skyModel	  = EDITOR_MODEL_SKYSPHERE_ID;
		m_world->GetGfxSettings().skyMaterial = EDITOR_MATERIAL_DEFAULT_SKY_ID;

		const HashSet<ResourceID> initialResources = {
			EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID,
			EDITOR_SHADER_DEFAULT_SKY_ID,
			m_resource->GetID(),
		};

		m_world->LoadMissingResources(m_editor->GetApp()->GetResourceManager(), m_editor->GetProjectManager().GetProjectData(), initialResources);

		m_world->Initialize(m_resourceManager);

		ResourceManagerV2& rm = m_editor->GetApp()->GetResourceManager();

		Entity* floor = EditorWorldUtility::AddModelToWorld(m_world->ConsumeEntityGUID(), m_world, rm.GetResource<Model>(EDITOR_MODEL_PLANE_ID), {EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID});

		floor->GetPhysicsSettings().bodyType  = PhysicsBodyType::Static;
		floor->GetPhysicsSettings().shapeType = PhysicsShapeType::Plane;

		SetupWorld();

		StoreEditorActionBuffer();
		UpdateResourceProperties();
		RebuildContents();
	}

	void PanelPhysicsMaterialViewer::StoreEditorActionBuffer()
	{
		PhysicsMaterial* mat = static_cast<PhysicsMaterial*>(m_resource);
		m_previousStream.Destroy();
		mat->SaveToStream(m_previousStream);
	}

	void PanelPhysicsMaterialViewer::UpdateResourceProperties()
	{
		PhysicsMaterial* mat = static_cast<PhysicsMaterial*>(m_resource);
		m_materialName		 = m_resource->GetName();
		m_previousStream.Destroy();
		mat->SaveToStream(m_previousStream);
	}

	void PanelPhysicsMaterialViewer::SetupWorld()
	{
		if (m_world->GetPlayMode() == PlayMode::Physics)
			m_world->SetPlayMode(PlayMode::Physics);

		ResourceManagerV2& rm = m_editor->GetApp()->GetResourceManager();

		Resource* res = rm.GetIfExists(m_resource->GetTID(), m_resource->GetID());

		if (res == nullptr)
			return;
		PhysicsMaterial* mat = static_cast<PhysicsMaterial*>(m_resource);

		if (m_displayEntity)
			m_world->DestroyEntity(m_displayEntity);

		m_displayEntity = m_world->CreateEntity(m_world->ConsumeEntityGUID(), "");

		Material* displayMaterial = rm.GetResource<Material>(EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID);

		if (m_displayModel == DisplayModel::Cube)
		{
			m_displayEntity									= EditorWorldUtility::AddModelToWorld(m_world->ConsumeEntityGUID(), m_world, rm.GetResource<Model>(EDITOR_MODEL_CUBE_ID), {EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID});
			m_displayEntity->GetPhysicsSettings().shapeType = PhysicsShapeType::Box;
		}
		else if (m_displayModel == DisplayModel::Sphere)
		{
			m_displayEntity									= EditorWorldUtility::AddModelToWorld(m_world->ConsumeEntityGUID(), m_world, rm.GetResource<Model>(EDITOR_MODEL_SPHERE_ID), {EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID});
			m_displayEntity->GetPhysicsSettings().shapeType = PhysicsShapeType::Sphere;
		}

		m_displayEntity->SetPosition(Vector3(0.0f, 1.0f, 0.0f));
		m_displayEntity->GetPhysicsSettings().bodyType = PhysicsBodyType::Dynamic;

		m_world->SetPlayMode(PlayMode::Physics);
	}

	void PanelPhysicsMaterialViewer::RebuildContents()
	{
		PhysicsMaterial* mat = static_cast<PhysicsMaterial*>(m_resource);

		m_inspector->DeallocAllChildren();
		m_inspector->RemoveAllChildren();

		DirectionalLayout* panelItems = m_manager->Allocate<DirectionalLayout>();
		panelItems->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_Y_TOTAL_CHILDREN);
		panelItems->SetAlignedSize(Vector2::One);
		panelItems->SetAlignedPosX(0.0f);
		panelItems->GetWidgetProps().childPadding = m_inspector->GetWidgetProps().childPadding;
		panelItems->GetProps().direction		  = DirectionOrientation::Vertical;
		panelItems->GetCallbacks().onEditEnded	  = [this]() { SetupWorld(); };
		m_inspector->AddChild(panelItems);
		CommonWidgets::BuildClassReflection(panelItems, this, ReflectionSystem::Get().Resolve<PanelPhysicsMaterialViewer>());

		CommonWidgets::BuildSeperator(m_inspector);

		DirectionalLayout* matItems = m_manager->Allocate<DirectionalLayout>();
		matItems->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_Y_TOTAL_CHILDREN);
		matItems->SetAlignedSize(Vector2::One);
		matItems->SetAlignedPosX(0.0f);
		matItems->GetWidgetProps().childPadding = m_inspector->GetWidgetProps().childPadding;
		matItems->GetProps().direction			= DirectionOrientation::Vertical;
		matItems->GetCallbacks().onEditEnded	= [this]() {
			   UpdateMaterial();
			   SetupWorld();
		};
		m_inspector->AddChild(matItems);
		CommonWidgets::BuildClassReflection(matItems, mat, ReflectionSystem::Get().Resolve<PhysicsMaterial>());
	}

	void PanelPhysicsMaterialViewer::UpdateMaterial()
	{
		PhysicsMaterial* mat = static_cast<PhysicsMaterial*>(m_resource);
		OStream			 stream;
		mat->SaveToStream(stream);
		EditorActionResourcePhysicsMaterial::Create(m_editor, m_resource->GetID(), m_resourceSpace, m_previousStream, stream);
		stream.Destroy();
	}

} // namespace Lina::Editor
