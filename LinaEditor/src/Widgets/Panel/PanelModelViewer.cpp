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
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/Panel/PanelResourceBrowser.hpp"
#include "Editor/Widgets/Compound/ResourceDirectoryBrowser.hpp"
#include "Editor/Widgets/World/WorldDisplayer.hpp"
#include "Common/System/System.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Platform/PlatformProcess.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Layout/Popup.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Components/CameraComponent.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina::Editor
{
	void PanelModelViewer::Construct()
	{
		m_editor = Editor::Get();

		DirectionalLayout* horizontal = m_manager->Allocate<DirectionalLayout>("Horizontal");
		horizontal->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		horizontal->SetAlignedPos(Vector2::Zero);
		horizontal->SetAlignedSize(Vector2::One);
		horizontal->GetProps().direction = DirectionOrientation::Horizontal;
		horizontal->GetProps().mode		 = DirectionalLayout::Mode::Bordered;
		AddChild(horizontal);

		Widget* worldBG = m_manager->Allocate<Widget>("ModelBG");
		worldBG->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		worldBG->SetAlignedPos(Vector2(0.0f, 0.0f));
		worldBG->SetAlignedSize(Vector2(0.75f, 1.0f));
		worldBG->GetWidgetProps().childMargins	  = TBLR::Eq(Theme::GetDef().baseIndent);
		worldBG->GetWidgetProps().drawBackground  = true;
		worldBG->GetWidgetProps().colorBackground = Theme::GetDef().background0;
		worldBG->GetWidgetProps().childMargins	  = TBLR::Eq(Theme::GetDef().baseIndent);
		horizontal->AddChild(worldBG);

		WorldDisplayer* displayer = m_manager->Allocate<WorldDisplayer>("WorldDisplayer");
		displayer->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		displayer->SetAlignedPos(Vector2::Zero);
		displayer->SetAlignedSize(Vector2::One);
		worldBG->AddChild(displayer);

		DirectionalLayout* right = m_manager->Allocate<DirectionalLayout>("RightSide");
		right->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		right->SetAlignedPos(Vector2(0.75f, 0.0f));
		right->SetAlignedSize(Vector2(0.25f, 1.0f));
		right->GetProps().direction = DirectionOrientation::Vertical;
		right->GetProps().mode		= DirectionalLayout::Mode::Bordered;
		horizontal->AddChild(right);

		Widget* inspectorParent = m_manager->Allocate<Widget>("InspectorParent");
		inspectorParent->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		inspectorParent->SetAlignedPos(Vector2(0.0f, 0.0f));
		inspectorParent->SetAlignedSize(Vector2(1.0f, 0.5f));
		right->AddChild(inspectorParent);

		Widget* hierarchyParent = m_manager->Allocate<Widget>("HierarchyParent");
		hierarchyParent->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		hierarchyParent->SetAlignedPos(Vector2(0.0f, 0.5f));
		hierarchyParent->SetAlignedSize(Vector2(1.0f, 0.5f));
		right->AddChild(hierarchyParent);

		// TODO: Add hierarchy browser here.

		ScrollArea* scrollInspector = m_manager->Allocate<ScrollArea>("Scroll");
		scrollInspector->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scrollInspector->SetAlignedPos(Vector2::Zero);
		scrollInspector->SetAlignedSize(Vector2::One);
		scrollInspector->GetProps().direction = DirectionOrientation::Vertical;
		inspectorParent->AddChild(scrollInspector);

		DirectionalLayout* inspector = m_manager->Allocate<DirectionalLayout>("Inspector");
		inspector->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		inspector->SetAlignedPos(Vector2::Zero);
		inspector->SetAlignedSize(Vector2::One);
		inspector->GetProps().direction				  = DirectionOrientation::Vertical;
		inspector->GetWidgetProps().childPadding	  = Theme::GetDef().baseIndentInner;
		inspector->GetWidgetProps().clipChildren	  = true;
		inspector->GetWidgetProps().childMargins.left = Theme::GetDef().baseBorderThickness;
		inspector->GetWidgetProps().childMargins.top  = Theme::GetDef().baseIndent;
		scrollInspector->AddChild(inspector);

		m_worldDisplayer = displayer;
		m_inspector		 = inspector;
	}

	void PanelModelViewer::Initialize()
	{
		if (m_model != nullptr)
			return;

		if (!m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResource(m_subData))
		{
			Text* text = m_manager->Allocate<Text>("Info");
			text->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			text->SetAlignedPos(Vector2(0.5f, 0.5f));
			text->SetAnchorX(Anchor::Center);
			text->SetAnchorY(Anchor::Center);
			text->GetProps().text = Locale::GetStr(LocaleStr::ThisResourceNoLongerExists);
			m_worldDisplayer->AddChild(text);
			m_worldDisplayer->DisplayWorld(nullptr);
			return;
		}

		ResourceDef def = {
			.id	 = m_subData,
			.tid = GetTypeID<Model>(),
		};
		m_editor->GetResourceManagerV2().LoadResourcesFromProject(m_editor, m_editor->GetProjectManager().GetProjectData(), {def}, 0);
		m_editor->GetResourceManagerV2().WaitForAll();
		m_model					   = m_editor->GetResourceManagerV2().GetResource<Model>(def.id);
		GetWidgetProps().debugName = "Model: " + m_model->GetName();

		FoldLayout* foldGeneral = CommonWidgets::BuildFoldTitle(this, Locale::GetStr(LocaleStr::General), &m_generalFold);
		FoldLayout* foldModel	= CommonWidgets::BuildFoldTitle(this, "Model", &m_modelFold);
		m_inspector->AddChild(foldGeneral);
		m_inspector->AddChild(foldModel);

		CommonWidgets::BuildClassReflection(foldGeneral, this, ReflectionSystem::Get().Resolve<PanelModelViewer>(), [this](const MetaType& meta, FieldBase* field) {

		});

		CommonWidgets::BuildClassReflection(foldModel, &m_model->GetMeta(), ReflectionSystem::Get().Resolve<Model::Metadata>(), [this](const MetaType& meta, FieldBase* field) {
			SetRuntimeDirty(true);
			// RegenModel("");
		});

		auto buildButtonLayout = [this]() -> Widget* {
			DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>();
			layout->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
			layout->SetAlignedPosX(0.0f);
			layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
			layout->SetAlignedSizeX(1.0f);
			layout->GetProps().mode						= DirectionalLayout::Mode::EqualSizes;
			layout->GetProps().direction				= DirectionOrientation::Horizontal;
			layout->GetWidgetProps().childMargins.left	= Theme::GetDef().baseIndent;
			layout->GetWidgetProps().childMargins.right = Theme::GetDef().baseIndent;
			layout->GetWidgetProps().childPadding		= Theme::GetDef().baseIndent;
			return layout;
		};

		Widget* buttonLayout1 = buildButtonLayout();
		Widget* buttonLayout2 = buildButtonLayout();
		foldGeneral->AddChild(buttonLayout1);
		foldGeneral->AddChild(buttonLayout2);
		static_cast<DirectionalLayout*>(buttonLayout1)->GetProps().mode = DirectionalLayout::Mode::EqualSizes;
		static_cast<DirectionalLayout*>(buttonLayout2)->GetProps().mode = DirectionalLayout::Mode::EqualSizes;

		Button* importButton = WidgetUtility::BuildIconTextButton(this, ICON_IMPORT, Locale::GetStr(LocaleStr::Import));
		importButton->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		importButton->SetAlignedPosY(0.0f);
		importButton->SetAlignedSizeY(1.0f);
		importButton->SetFixedSizeX(Theme::GetDef().baseItemHeight * 4);
		importButton->GetProps().onClicked = [this]() {
			const Vector<String> paths = PlatformProcess::OpenDialog({
				.title				   = Locale::GetStr(LocaleStr::Import),
				.primaryButton		   = Locale::GetStr(LocaleStr::Import),
				.extensionsDescription = "",
				.extensions			   = {"ttf", "otf"},
				.mode				   = PlatformProcess::DialogMode::SelectFile,
			});
			if (paths.empty())
				return;

			RegenModel(paths.front());
			SetRuntimeDirty(true);
		};
		buttonLayout1->AddChild(importButton);

		Button* reimportButton = WidgetUtility::BuildIconTextButton(this, ICON_ROTATE, Locale::GetStr(LocaleStr::ReImport));
		reimportButton->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		reimportButton->SetAlignedPosY(0.0f);
		reimportButton->SetAlignedSizeY(1.0f);
		reimportButton->SetFixedSizeX(Theme::GetDef().baseItemHeight * 4);
		reimportButton->GetProps().onClicked = [this, reimportButton]() {
			const String path = m_model->GetPath();
			if (!FileSystem::FileOrPathExists(path))
			{
				CommonWidgets::ThrowInfoTooltip(Locale::GetStr(LocaleStr::FileNotFound), LogLevel::Error, 3.0f, reimportButton);
				return;
			}

			RegenModel(path);
			SetRuntimeDirty(true);
		};

		buttonLayout1->AddChild(reimportButton);

		Button* save = WidgetUtility::BuildIconTextButton(this, ICON_SAVE, Locale::GetStr(LocaleStr::Save));
		save->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		save->SetAlignedPosY(0.0f);
		save->SetAlignedSizeY(1.0f);
		save->SetFixedSizeX(Theme::GetDef().baseItemHeight * 4);
		save->GetProps().onClicked = [this]() {
			if (m_containsRuntimeChanges)
			{
				m_resourceManager->SaveResource(m_editor->GetProjectManager().GetProjectData(), m_model);
				m_editor->GetResourcePipeline().GenerateThumbnailForResource(m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResource(m_model->GetID()), m_model, true);
				DockArea* outDockArea = nullptr;
				Panel*	  p			  = m_editor->GetWindowPanelManager().FindPanelOfType(PanelType::ResourceBrowser, 0, outDockArea);
				if (p)
					static_cast<PanelResourceBrowser*>(p)->GetBrowser()->RefreshDirectory();
				SetRuntimeDirty(false);
			}
		};

		m_saveButton = save;
		buttonLayout2->AddChild(save);

		// Setup world
		m_world			= new EntityWorld(0, "");
		m_lastWorldSize = Vector2ui(4, 4);
		m_worldRenderer = new WorldRenderer(m_world, m_lastWorldSize);

		Vector<ResourceDef> neededResources = {{
												   .id	 = DEFAULT_SHADER_LIGHTING_ID,
												   .name = DEFAULT_SHADER_LIGHTING_PATH,
												   .tid	 = GetTypeID<Shader>(),
											   },
											   {
												   .id	 = DEFAULT_SHADER_SKY_ID,
												   .name = DEFAULT_SHADER_SKY_PATH,
												   .tid	 = GetTypeID<Shader>(),
											   },
											   {
												   .id	 = DEFAULT_SKY_CUBE_ID,
												   .name = DEFAULT_SKY_CUBE_PATH,
												   .tid	 = GetTypeID<Model>(),
											   }};
		m_world->GetResourceManagerV2().LoadResourcesFromFile(m_editor, neededResources, -1);
		m_world->GetResourceManagerV2().LoadResourcesFromProject(m_editor, m_editor->GetProjectManager().GetProjectData(), {{.id = m_model->GetID(), .name = m_model->GetName(), .tid = GetTypeID<Model>()}}, -1);
		m_world->GetResourceManagerV2().WaitForAll();

		SetupScene();

		// Add rendering
		m_editor->GetEditorRenderer().AddWorldRenderer(m_worldRenderer);

		m_worldDisplayer->DisplayWorld(m_worldRenderer);
		Panel::Initialize();
	}

	void PanelModelViewer::SetupScene()
	{
		Material* skyMaterial	   = m_world->GetResourceManagerV2().CreateResource<Material>(m_world->GetResourceManagerV2().ConsumeResourceID(), "Model Viewer Sky Material");
		Material* lightingMaterial = m_world->GetResourceManagerV2().CreateResource<Material>(m_world->GetResourceManagerV2().ConsumeResourceID(), "Model Viewer Lighting Material");
		Shader*	  lightingShader   = m_world->GetResourceManagerV2().GetResource<Shader>(DEFAULT_SHADER_LIGHTING_ID);
		Shader*	  skyShader		   = m_world->GetResourceManagerV2().GetResource<Shader>(DEFAULT_SHADER_SKY_ID);
		lightingMaterial->SetShader(lightingShader);
		skyMaterial->SetShader(skyShader);
		m_world->GetGfxSettings().SetSkyMaterial(skyMaterial);
		m_world->GetGfxSettings().SetLightingMaterial(lightingMaterial);

		Entity*			 camera		= m_world->CreateEntity("Camera");
		CameraComponent* cameraComp = m_world->AddComponent<CameraComponent>(camera);
		m_world->SetActiveCamera(cameraComp);

		Entity* model = m_world->AddModelToWorld(m_model);
	}

	void PanelModelViewer::Destruct()
	{
		Panel::Destruct();
		if (m_model == nullptr)
			return;

		m_editor->GetResourceManagerV2().UnloadResources({m_model});
		m_editor->GetEditorRenderer().RemoveWorldRenderer(m_worldRenderer);
		delete m_worldRenderer;
		delete m_world;
	}

	void PanelModelViewer::PreTick()
	{
		if (m_world == nullptr)
			return;

		const Vector2ui sz = m_worldDisplayer->GetSize();

		if (sz.x != 0 && sz.y != 0 && !sz.Equals(m_lastWorldSize))
		{
			m_lastWorldSize = sz;
			m_editor->GetGfxManager()->Join();
			m_worldRenderer->Resize(m_lastWorldSize);
			m_editor->GetEditorRenderer().RefreshDynamicTextures();
		}

		m_world->PreTick();
	}

	void PanelModelViewer::Tick(float delta)
	{
		if (m_world == nullptr)
			return;

		m_world->Tick(delta);
	}

	void PanelModelViewer::SetRuntimeDirty(bool isDirty)
	{
		m_containsRuntimeChanges = isDirty;
		Text* txt				 = GetWidgetOfType<Text>(m_saveButton);
		if (isDirty)
			txt->GetProps().text = Locale::GetStr(LocaleStr::Save) + " *";
		else
			txt->GetProps().text = Locale::GetStr(LocaleStr::Save);
		txt->CalculateTextSize();
	}

	void PanelModelViewer::RegenModel(const String& path)
	{
		/*
		m_editor->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager)->Join();
		m_font->DestroyHW();

		if (!path.empty())
			m_font->LoadFromFile(path);

		m_font->GenerateHW(m_editor->GetEditorRenderer().GetGUIBackend().GetLVGText());
		m_editor->GetEditorRenderer().MarkBindlessDirty();
		m_fontDisplay->GetProps().font = m_font->GetID();
		m_fontDisplay->CalculateTextSize();*/
	}

	void PanelModelViewer::SaveLayoutToStream(OStream& stream)
	{
	}

	void PanelModelViewer::LoadLayoutFromStream(IStream& stream)
	{
	}
} // namespace Lina::Editor
