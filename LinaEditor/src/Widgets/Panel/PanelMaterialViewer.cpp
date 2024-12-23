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

#include "Editor/Widgets/Panel/PanelMaterialViewer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/World/WorldDisplayer.hpp"
#include "Editor/Widgets/Compound/ColorWheelCompound.hpp"
#include "Editor/Actions/EditorActionResources.hpp"
#include "Editor/Graphics/GridRenderer.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Checkbox.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Application.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/World/WorldUtility.hpp"

namespace Lina::Editor
{

	void PanelMaterialViewer::Construct()
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

	void PanelMaterialViewer::Initialize()
	{
		PanelResourceViewer::Initialize();

		if (!m_resource)
			return;

		if (m_world)
			return;

		m_world			= new EntityWorld(0, "");
		m_worldRenderer = new WorldRenderer(&m_editor->GetApp()->GetGfxContext(), &m_editor->GetApp()->GetResourceManager(), m_world, Vector2ui(4, 4), "WorldRenderer: " + m_resource->GetName() + " :");
		m_gridRenderer	= new GridRenderer(m_editor, m_editor->GetApp()->GetLGX(), m_world, m_worldRenderer, &m_editor->GetApp()->GetResourceManager());
		m_worldRenderer->AddFeatureRenderer(m_gridRenderer);

		m_editor->GetApp()->JoinRender();

		m_editor->GetApp()->GetWorldProcessor().AddWorld(m_world);
		m_editor->GetEditorRenderer().AddWorldRenderer(m_worldRenderer);

		m_worldDisplayer->DisplayWorld(m_worldRenderer, WorldDisplayer::WorldCameraType::Orbit);

		// Resource set up.
		m_world->GetGfxSettings().lightingMaterial = EDITOR_MATERIAL_DEFAULT_LIGHTING_ID;
		m_world->GetGfxSettings().skyModel		   = EDITOR_MODEL_SKYSPHERE_ID;
		m_world->GetGfxSettings().skyMaterial	   = EDITOR_MATERIAL_DEFAULT_SKY_ID;

		const HashSet<ResourceID> initialResources = {
			EDITOR_MODEL_CUBE_ID,
			EDITOR_MODEL_SPHERE_ID,
			EDITOR_MODEL_PLANE_ID,
			EDITOR_MODEL_CYLINDER_ID,
			EDITOR_MODEL_CAPSULE_ID,
			EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID,
			EDITOR_SHADER_DEFAULT_SKY_ID,
			m_resource->GetID(),
		};

		m_world->LoadMissingResources(m_editor->GetApp()->GetResourceManager(), m_editor->GetProjectManager().GetProjectData(), initialResources, m_resourceSpace);

		if (!m_defaultSky)
		{
			m_defaultSky = m_resourceManager->CreateResource<Material>(m_resourceManager->ConsumeResourceID(), "MaterialViewerSkyMaterial", m_resourceSpace);
			WorldUtility::SetupDefaultSkyMaterial(m_defaultSky, m_resourceManager);
			m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
		}

		m_world->Initialize(m_resourceManager);
		SetupWorld();

		StoreShaderID();
		StoreEditorActionBuffer();
		UpdateResourceProperties();
		RebuildContents();
	}

	void PanelMaterialViewer::Destruct()
	{
		PanelResourceViewer::Destruct();

		m_previousStream.Destroy();

		if (m_world)
		{
			m_editor->GetApp()->JoinRender();
			m_editor->GetApp()->GetWorldProcessor().RemoveWorld(m_world);
			m_editor->GetEditorRenderer().RemoveWorldRenderer(m_worldRenderer);
			delete m_worldRenderer;
			delete m_gridRenderer;
			delete m_world;
			m_worldRenderer = nullptr;
			m_world			= nullptr;
		}
	}

	void PanelMaterialViewer::StoreShaderID()
	{
		Material* mat	   = static_cast<Material*>(m_resource);
		m_previousShaderID = mat->GetShader();
	}

	void PanelMaterialViewer::StoreEditorActionBuffer()
	{
		Material* mat = static_cast<Material*>(m_resource);
		m_previousStream.Destroy();
		mat->SaveToStream(m_previousStream);
	}

	void PanelMaterialViewer::UpdateResourceProperties()
	{
		Material* mat  = static_cast<Material*>(m_resource);
		m_materialName = m_resource->GetName();
		m_shaderType   = mat->GetShaderType();

		if (mat->GetShaderType() == ShaderType::DeferredSurface)
			m_shaderTypeStr = "Opaque Surface";
		else if (mat->GetShaderType() == ShaderType::ForwardSurface)
			m_shaderTypeStr = "Transparent Surface";
		else if (mat->GetShaderType() == ShaderType::Sky)
			m_shaderTypeStr = "Sky";
		else
			m_shaderTypeStr = "Custom";

		m_propertyFoldValues.resize(mat->GetProperties().size());
		m_previousStream.Destroy();
		mat->SaveToStream(m_previousStream);
		m_shaderID = mat->GetShader();
	}

	void PanelMaterialViewer::SetupWorld()
	{
		ResourceManagerV2& rm = m_editor->GetApp()->GetResourceManager();

		Resource* res = rm.GetIfExists(m_resource->GetTID(), m_resource->GetID());

		if (res == nullptr)
			return;

		if (m_displayEntity != nullptr)
			m_world->DestroyEntity(m_displayEntity);
		m_displayEntity = nullptr;

		Material*		 mat		= static_cast<Material*>(m_resource);
		const ShaderType shaderType = mat->GetShaderType();

		if (shaderType == ShaderType::Sky)
		{
			m_world->GetGfxSettings().skyMaterial = mat->GetID();
		}
		else if (shaderType == ShaderType::Lighting)
		{
			m_world->GetGfxSettings().lightingMaterial = mat->GetID();
		}
		else
		{
			m_world->GetGfxSettings().skyMaterial = m_defaultSky->GetID();
		}

		const ResourceID displayMaterial = shaderType == ShaderType::Sky ? EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID : m_resource->GetID();

		if (m_displayType == MaterialViewerDisplayType::Cube)
			m_displayEntity = WorldUtility::AddModelToWorld(m_world, rm.GetResource<Model>(EDITOR_MODEL_CUBE_ID), {displayMaterial});
		else if (m_displayType == MaterialViewerDisplayType::Sphere)
			m_displayEntity = WorldUtility::AddModelToWorld(m_world, rm.GetResource<Model>(EDITOR_MODEL_SPHERE_ID), {displayMaterial});
		if (m_displayType == MaterialViewerDisplayType::Cylinder)
			m_displayEntity = WorldUtility::AddModelToWorld(m_world, rm.GetResource<Model>(EDITOR_MODEL_CYLINDER_ID), {displayMaterial});
		if (m_displayType == MaterialViewerDisplayType::Capsule)
			m_displayEntity = WorldUtility::AddModelToWorld(m_world, rm.GetResource<Model>(EDITOR_MODEL_CAPSULE_ID), {displayMaterial});
		if (m_displayType == MaterialViewerDisplayType::Plane)
			m_displayEntity = WorldUtility::AddModelToWorld(m_world, rm.GetResource<Model>(EDITOR_MODEL_PLANE_ID), {displayMaterial});
	}

	void PanelMaterialViewer::RebuildContents()
	{
		m_inspector->DeallocAllChildren();
		m_inspector->RemoveAllChildren();

		CommonWidgets::BuildClassReflection(m_inspector, this, ReflectionSystem::Get().Resolve<PanelMaterialViewer>(), [this](MetaType* meta, FieldBase* field) { SetupWorld(); });
		Material* mat = static_cast<Material*>(m_resource);

		Widget* shaderField			= CommonWidgets::BuildFieldLayoutWithRightSide(m_inspector, 0, Locale::GetStr(LocaleStr::Shader), false, nullptr, 0.6f);
		Widget* shaderResourceField = CommonWidgets::BuildResourceField(m_inspector, &m_shaderID, GetTypeID<Shader>(), [this, mat](ResourceDirectory* selected) {
			m_shaderID = selected->resourceID;
			EditorActionResourceMaterialShader::Create(m_editor, mat->GetID(), m_resourceSpace, m_previousShaderID, m_shaderID, m_previousStream);
		});

		Widget::GetWidgetOfType<DirectionalLayout>(shaderField)->AddChild(shaderResourceField);
		m_inspector->AddChild(shaderField);

		auto buildColorButton = [this](uint8* data, uint8 comps) -> Button* {
			Button* b = m_manager->Allocate<Button>();
			b->GetFlags().Set(WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y);
			b->SetAlignedPosY(0.0f);
			b->SetAlignedSize(Vector2::One);
			b->CreateIcon(ICON_PALETTE);
			b->GetWidgetProps().tooltip = Locale::GetStr(LocaleStr::SelectColor);
			b->GetProps().onClicked		= [data, comps, this, b]() {
				PanelColorWheel* panel = Editor::Get()->GetWindowPanelManager().OpenColorWheelPanel(b);
				panel->SetListener(this);
				m_colorWheel = panel;

				if (comps == 3)
				{
					Vector3* v = reinterpret_cast<Vector3*>(data);
					panel->GetWheel()->SetTargetColor(Color(v->x, v->y, v->z, 1.0f), false);
					panel->GetWheel()->GetProps().trackColorv3 = v;
				}
				else if (comps == 4)
				{
					Vector4* v = reinterpret_cast<Vector4*>(data);
					panel->GetWheel()->SetTargetColor(Color(v->x, v->y, v->z, v->w), false);
					panel->GetWheel()->GetProps().trackColorv4 = v;
				}

				m_latestColorWheelComps = comps;
				m_latestColorWheelData	= data;
			};
			return b;
		};
		auto buildFloatField = [this](Widget* srcToAdd, uint8* dataStart, uint8 fieldCount, bool isInt, bool isUnsigned) {
			for (uint8 i = 0; i < fieldCount; i++)
			{
				InputField* inp				   = CommonWidgets::BuildFloatField(m_inspector, dataStart + i * sizeof(uint32), 32, isInt, isUnsigned, true, isUnsigned ? 0 : -1000.0f, 1000.0f, 0.1f);
				inp->GetProps().onEditEnd	   = [this](const String& txt) { UpdateMaterial(); };
				inp->GetProps().onValueChanged = [this](float val, bool fromSlider) {
					if (fromSlider)
						m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
				};
				inp->GetProps().disableNumberSlider = true;
				srcToAdd->AddChild(inp);
			}
		};

		auto buildDefaultLayout = [this](const String& name) -> DirectionalLayout* {
			DirectionalLayout* layout	 = static_cast<DirectionalLayout*>(CommonWidgets::BuildFieldLayoutWithRightSide(m_inspector, 0, name, false, nullptr, 0.6f));
			DirectionalLayout* rightSide = Widget::GetWidgetOfType<DirectionalLayout>(layout);
			m_inspector->AddChild(layout);
			return rightSide;
		};

		uint32 i = 0;

		m_propertyFoldValues.resize(mat->GetProperties().size());

		for (MaterialProperty* p : mat->GetProperties())
		{
			if (p->propDef.type == ShaderPropertyType::Float)
			{
				DirectionalLayout* rightSide = buildDefaultLayout(p->propDef.name);
				buildFloatField(rightSide, p->data.data(), 1, false, false);
			}
			else if (p->propDef.type == ShaderPropertyType::Int32)
			{
				DirectionalLayout* rightSide = buildDefaultLayout(p->propDef.name);
				buildFloatField(rightSide, p->data.data(), 1, true, false);
			}
			else if (p->propDef.type == ShaderPropertyType::UInt32)
			{
				DirectionalLayout* rightSide = buildDefaultLayout(p->propDef.name);
				buildFloatField(rightSide, p->data.data(), 1, true, true);
			}
			else if (p->propDef.type == ShaderPropertyType::Bool)
			{
				DirectionalLayout* rightSide = buildDefaultLayout(p->propDef.name);
				Checkbox*		   v0		 = m_manager->Allocate<Checkbox>();
				v0->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y);
				v0->SetAlignedSize(Vector2::One);
				v0->SetAlignedPosY(0.0f);
				v0->GetProps().value		   = reinterpret_cast<bool*>(p->data.data());
				v0->GetIcon()->GetProps().icon = ICON_CHECK;
				v0->GetIcon()->CalculateIconSize();
				v0->GetProps().onValueChanged = [p, this](bool val) {
					MEMCPY(p->data.data(), &val, sizeof(bool));
					UpdateMaterial();
				};

				rightSide->AddChild(v0);
			}
			else if (p->propDef.type == ShaderPropertyType::Vec2)
			{
				DirectionalLayout* rightSide = buildDefaultLayout(p->propDef.name);
				buildFloatField(rightSide, p->data.data(), 2, false, false);
			}
			else if (p->propDef.type == ShaderPropertyType::Vec3)
			{
				DirectionalLayout* rightSide = buildDefaultLayout(p->propDef.name);
				rightSide->AddChild(buildColorButton(p->data.data(), 3));
				buildFloatField(rightSide, p->data.data(), 3, false, false);
			}
			else if (p->propDef.type == ShaderPropertyType::Vec4)
			{
				DirectionalLayout* rightSide = buildDefaultLayout(p->propDef.name);
				rightSide->AddChild(buildColorButton(p->data.data(), 4));
				buildFloatField(rightSide, p->data.data(), 4, false, false);
			}
			else if (p->propDef.type == ShaderPropertyType::IVec2)
			{
				DirectionalLayout* rightSide = buildDefaultLayout(p->propDef.name);
				buildFloatField(rightSide, p->data.data(), 2, true, false);
			}
			else if (p->propDef.type == ShaderPropertyType::IVec3)
			{
				DirectionalLayout* rightSide = buildDefaultLayout(p->propDef.name);
				buildFloatField(rightSide, p->data.data(), 3, true, false);
			}
			else if (p->propDef.type == ShaderPropertyType::IVec4)
			{
				DirectionalLayout* rightSide = buildDefaultLayout(p->propDef.name);
				buildFloatField(rightSide, p->data.data(), 4, true, false);
			}
			else if (p->propDef.type == ShaderPropertyType::Matrix4)
			{
				FoldLayout* fold = static_cast<FoldLayout*>(CommonWidgets::BuildFieldLayout(this, 0, "Matrix4", true, reinterpret_cast<bool*>(&m_propertyFoldValues[i])));
				m_inspector->AddChild(fold);

				for (uint8 i = 0; i < 4; i++)
				{
					DirectionalLayout* row = static_cast<DirectionalLayout*>(CommonWidgets::BuildFieldLayoutWithRightSide(this, 0, "Row1", false, nullptr));
					fold->AddChild(row);

					DirectionalLayout* rowRightSide = Widget::GetWidgetOfType<DirectionalLayout>(row);
					buildFloatField(rowRightSide, p->data.data() + sizeof(float) * i * 4, 4, false, false);
				}
			}
			else if (p->propDef.type == ShaderPropertyType::Texture2D)
			{
				LinaTexture2D* txt = reinterpret_cast<LinaTexture2D*>(p->data.data());

				Widget* txtField = CommonWidgets::BuildTextureField(this, m_editor->GetProjectManager().GetProjectData(), txt, 0, p->propDef.name, reinterpret_cast<bool*>(&m_propertyFoldValues[i]), [this]() { UpdateMaterial(); });
				m_inspector->AddChild(txtField);
			}

			i++;
		}

		m_inspector->Initialize();

		if (m_previewOnly)
			DisableRecursively(m_inspector);
	}

	void PanelMaterialViewer::OnPanelColorWheelValueChanged(const Color& linearColor)
	{
		Color val = linearColor;
		for (uint8 i = 0; i < m_latestColorWheelComps; i++)
			MEMCPY(m_latestColorWheelData + sizeof(float) * i, &val[i], sizeof(float));

		// Update without commands.
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
	}

	void PanelMaterialViewer::OnPanelColorWheelListenerChanged()
	{
		UpdateMaterial();
	}

	void PanelMaterialViewer::OnPanelColorWheelClosed()
	{
		UpdateMaterial();
		m_colorWheel = nullptr;
	}

	void PanelMaterialViewer::UpdateMaterial()
	{
		Material* mat = static_cast<Material*>(m_resource);

		if (m_colorWheel)
			m_colorWheel->GetWheel()->FetchFromTrackedColors();

		OStream stream;
		mat->SaveToStream(stream);
		EditorActionResourceMaterial::Create(m_editor, m_resource->GetID(), m_resourceSpace, m_previousStream, stream);
		stream.Destroy();
	}

} // namespace Lina::Editor
