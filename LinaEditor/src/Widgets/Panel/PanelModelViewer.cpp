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
#include "Editor/Actions/EditorActionResources.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/Popup.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/World/Components/CompModel.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Resources/ResourceDirectory.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Editor/Graphics/EditorWorldRenderer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Core/Application.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/World/EditorWorldUtility.hpp"

namespace Lina::Editor
{
	void PanelModelViewer::Construct()
	{
		PanelResourceViewer::Construct();

		m_resourceBG->GetWidgetProps().childMargins = TBLR::Eq(0.0f);
		m_worldDisplayer							= m_manager->Allocate<WorldDisplayer>("WorldDisplayer");
		m_worldDisplayer->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_worldDisplayer->SetAlignedPos(Vector2::Zero);
		m_worldDisplayer->SetAlignedSize(Vector2::One);

		m_resourceBG->AddChild(m_worldDisplayer);
		m_displayAnimation = m_editor->GetSettings().GetParams().GetParamInt32("AnimationPreview"_hs, -1);

		m_worldDisplayer->GetProps().noWorldText = Locale::GetStr(LocaleStr::ResourceNotFound);
	}

	void PanelModelViewer::Initialize()
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

		HashSet<ResourceID> initialResources = {
			ENGINE_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID,
			m_resource->GetID(),
		};

		Model* model = static_cast<Model*>(m_resource);
		for (ResourceID id : model->GetMeta().materials)
			initialResources.insert(id);

		if (m_displayAnimation >= static_cast<int32>(model->GetAllAnimations().size()))
		{
			m_displayAnimation = -1;
			m_editor->GetSettings().GetParams().SetParamInt32("AnimationPreview"_hs, -1);
			m_editor->SaveSettings();
		}

		m_world->LoadMissingResources(m_editor->GetApp()->GetResourceManager(), m_editor->GetProjectManager().GetProjectData(), initialResources);
		m_world->Initialize(m_resourceManager);

		if (!m_skyMaterial)
		{
			m_skyMaterial = m_resourceManager->CreateResource<Material>(m_resourceManager->ConsumeResourceID(), "ModelViewerSkyMaterial", m_resourceSpace);
			EditorWorldUtility::SetupDefaultSkyMaterial(m_skyMaterial, m_resourceManager);
			m_world->GetGfxSettings().skyMaterial = m_skyMaterial->GetID();
			m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
		}

		SetupWorld();

		UpdateResourceProperties();
		RebuildContents();
	}

	void PanelModelViewer::Destruct()
	{

		PanelResourceViewer::Destruct();

		m_previousStream.Destroy();

		if (m_world)
			m_editor->GetWorldManager().DestroyEditorWorld(m_world);

		m_world = nullptr;
	}

	void PanelModelViewer::StoreEditorActionBuffer()
	{
		Model* model = static_cast<Model*>(m_resource);
		m_previousStream.Destroy();
		model->SaveToStream(m_previousStream);
	}

	void PanelModelViewer::UpdateResourceProperties()
	{
		Model* model   = static_cast<Model*>(m_resource);
		m_modelName	   = model->GetName();
		m_animations   = "0";
		m_materialDefs = TO_STRING(model->GetMaterialDefs().size());
		m_meshes	   = TO_STRING(model->GetAllMeshes().size());
		m_previousStream.Destroy();
		model->SaveToStream(m_previousStream);
	}

	void PanelModelViewer::RebuildContents()
	{
		Model* model = static_cast<Model*>(m_resource);

		m_inspector->DeallocAllChildren();
		m_inspector->RemoveAllChildren();

		DirectionalLayout* panelItems = m_manager->Allocate<DirectionalLayout>();
		panelItems->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_Y_TOTAL_CHILDREN);
		panelItems->SetAlignedSize(Vector2::One);
		panelItems->SetAlignedPosX(0.0f);
		panelItems->GetWidgetProps().childPadding = m_inspector->GetWidgetProps().childPadding;
		panelItems->GetProps().direction		  = DirectionOrientation::Vertical;
		panelItems->GetCallbacks().onEditEnded	  = [this]() { m_compModel->GetAnimationController().SetSpeed(m_animationPreviewSpeed); };
		m_inspector->AddChild(panelItems);
		CommonWidgets::BuildClassReflection(panelItems, this, ReflectionSystem::Get().Resolve<PanelModelViewer>());

		CommonWidgets::BuildSeperator(m_inspector);

		Widget* animLayout = CommonWidgets::BuildFieldLayout(this, 0, Locale::GetStr(LocaleStr::PreviewAnimation), false);
		Widget* rightSide  = animLayout->GetChildren().back();

		Dropdown* animationDD = m_manager->Allocate<Dropdown>("Animation DD");
		animationDD->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		animationDD->SetAlignedPos(Vector2::Zero);
		animationDD->SetAlignedSize(Vector2(1.0f, 1.0f));

		animationDD->GetText()->UpdateTextAndCalcSize(m_displayAnimation == -1 ? Locale::GetStr(LocaleStr::None) : model->GetAllAnimations().at(m_displayAnimation).name);

		animationDD->GetProps().onAddItems = [model, this](Popup* popup) {
			const Vector<ModelAnimation>& anims = model->GetAllAnimations();

			popup->AddToggleItem(Locale::GetStr(LocaleStr::None), m_displayAnimation == -1, -1);
			int32 i = 0;
			for (const ModelAnimation& anim : anims)
			{
				popup->AddToggleItem(anim.name.empty() ? "NoName" : anim.name, m_displayAnimation == i, i);
				i++;
			}
		};

		animationDD->GetProps().onSelected = [this, model](int32 selection, String& newTitle) -> bool {
			const Vector<ModelAnimation>& anims = model->GetAllAnimations();
			m_displayAnimation					= selection;
			m_compModel->GetAnimationController().SelectAnimation(m_displayAnimation);
			newTitle = m_displayAnimation == -1 ? Locale::GetStr(LocaleStr::None) : anims.at(m_displayAnimation).name;
			if (newTitle.empty())
				newTitle = "NoName";
			m_editor->GetSettings().GetParams().SetParamInt32("AnimationPreview"_hs, m_displayAnimation);
			m_editor->SaveSettings();
			return true;
		};

		rightSide->AddChild(animationDD);
		m_inspector->AddChild(animLayout);

		Model::Metadata&	meta	  = model->GetMeta();
		Vector<ResourceID>& materials = meta.materials;

		const size_t mtSize = meta.materials.size();
		for (size_t i = 0; i < mtSize; i++)
		{
			Widget* matField = CommonWidgets::BuildField(m_inspector,
														 Locale::GetStr(LocaleStr::Material),
														 &materials[i],
														 {
															 .type = FieldType::ResourceID,
															 .tid  = GetTypeID<Material>(),
														 });

			matField->GetCallbacks().onEditEnded = [model, i, this]() {
				Model* model = static_cast<Model*>(m_resource);

				OStream stream;
				model->SaveToStream(stream);
				EditorActionResourceModel::Create(m_editor, m_resource->GetID(), m_resourceSpace, m_previousStream, stream);
				stream.Destroy();
			};

			m_inspector->AddChild(matField);
		}

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

		Model* model	= static_cast<Model*>(m_resource);
		m_displayEntity = EditorWorldUtility::AddModelToWorld(m_world->ConsumeEntityGUID(), m_world, model, model->GetMeta().materials);
		m_compModel		= m_world->GetComponent<CompModel>(m_displayEntity);
		m_compModel->GetAnimationController().SelectAnimation(m_displayAnimation);
		m_world->LoadMissingResources(m_editor->GetApp()->GetResourceManager(), m_editor->GetProjectManager().GetProjectData(), {});
		m_displayEntity->SetPosition(Vector3(0.0f, -0.1f, 0.0f));
	}

} // namespace Lina::Editor
