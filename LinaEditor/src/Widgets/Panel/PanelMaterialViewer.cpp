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
#include "Editor/Graphics/EditorWorldRenderer.hpp"
#include "Editor/Widgets/Panel/PanelColorWheel.hpp"
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
#include "Editor/World/EditorWorldUtility.hpp"

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

		EditorWorldRenderer* ewr = m_editor->GetWorldManager().CreateEditorWorld();
		m_world					 = ewr->GetWorldRenderer()->GetWorld();
		m_world->SetID(m_resourceSpace);
		m_worldDisplayer->DisplayWorld(ewr, WorldCameraType::Orbit);

		// Resource set up.
		m_world->GetGfxSettings().skyModel	  = EDITOR_MODEL_SKYSPHERE_ID;
		m_world->GetGfxSettings().skyMaterial = EDITOR_MATERIAL_DEFAULT_SKY_ID;

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

		m_world->LoadMissingResources(m_editor->GetApp()->GetResourceManager(), m_editor->GetProjectManager().GetProjectData(), initialResources);

		if (!m_defaultSky)
		{
			m_defaultSky = m_resourceManager->CreateResource<Material>(m_resourceManager->ConsumeResourceID(), "MaterialViewerSkyMaterial", m_resourceSpace);
			EditorWorldUtility::SetupDefaultSkyMaterial(m_defaultSky, m_resourceManager);
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
			m_editor->GetWorldManager().DestroyEditorWorld(m_world);
		m_world = nullptr;
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
		else
		{
			m_world->GetGfxSettings().skyMaterial = m_defaultSky->GetID();
		}

		const ResourceID displayMaterial = shaderType == ShaderType::Sky ? EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID : m_resource->GetID();

		if (m_displayType == MaterialViewerDisplayType::Cube)
			m_displayEntity = EditorWorldUtility::AddModelToWorld(m_world->ConsumeEntityGUID(), m_world, rm.GetResource<Model>(EDITOR_MODEL_CUBE_ID), {displayMaterial});
		else if (m_displayType == MaterialViewerDisplayType::Sphere)
			m_displayEntity = EditorWorldUtility::AddModelToWorld(m_world->ConsumeEntityGUID(), m_world, rm.GetResource<Model>(EDITOR_MODEL_SPHERE_ID), {displayMaterial});
		if (m_displayType == MaterialViewerDisplayType::Cylinder)
			m_displayEntity = EditorWorldUtility::AddModelToWorld(m_world->ConsumeEntityGUID(), m_world, rm.GetResource<Model>(EDITOR_MODEL_CYLINDER_ID), {displayMaterial});
		if (m_displayType == MaterialViewerDisplayType::Capsule)
			m_displayEntity = EditorWorldUtility::AddModelToWorld(m_world->ConsumeEntityGUID(), m_world, rm.GetResource<Model>(EDITOR_MODEL_CAPSULE_ID), {displayMaterial});
		if (m_displayType == MaterialViewerDisplayType::Plane)
			m_displayEntity = EditorWorldUtility::AddModelToWorld(m_world->ConsumeEntityGUID(), m_world, rm.GetResource<Model>(EDITOR_MODEL_PLANE_ID), {displayMaterial});

		m_displayEntity->SetPosition(Vector3(0.0f, -0.1f, 0.0f));
	}

	void PanelMaterialViewer::RebuildContents()
	{
		m_inspector->DeallocAllChildren();
		m_inspector->RemoveAllChildren();

		DirectionalLayout* panelItems = m_manager->Allocate<DirectionalLayout>();
		panelItems->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_Y_TOTAL_CHILDREN);
		panelItems->SetAlignedSize(Vector2::One);
		panelItems->SetAlignedPosX(0.0f);
		panelItems->GetWidgetProps().childPadding = m_inspector->GetWidgetProps().childPadding;
		panelItems->GetProps().direction		  = DirectionOrientation::Vertical;
		panelItems->GetCallbacks().onEditEnded	  = [this]() {
			   if (m_displayType == MaterialViewerDisplayType::Skybox && m_shaderType != ShaderType::Custom)
			   {
				   LINA_WARN("Can not display this shader type as a skybox.");
				   m_displayType = MaterialViewerDisplayType::Cube;
			   }
			   SetupWorld();
		};
		m_inspector->AddChild(panelItems);
		CommonWidgets::BuildClassReflection(panelItems, this, ReflectionSystem::Get().Resolve<PanelMaterialViewer>());

		Material* mat = static_cast<Material*>(m_resource);

		Widget* shaderField						= CommonWidgets::BuildField(m_inspector,
														Locale::GetStr(LocaleStr::Shader),
														&m_shaderID,
																			{
																				.type = FieldType::ResourceID,
																				.tid  = GetTypeID<Shader>(),
														});
		shaderField->GetCallbacks().onEditEnded = [this, mat]() {
			if (m_previousShaderID != m_shaderID)
				EditorActionResourceMaterialShader::Create(m_editor, mat->GetID(), m_resourceSpace, m_previousShaderID, m_shaderID, m_previousStream);
		};
		m_inspector->AddChild(shaderField);

		m_propertyFoldValues.resize(mat->GetProperties().size());

		/*

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

				inp->GetCallbacks().onEditEnded = [this](){
					UpdateMaterial();
				};

				inp->GetCallbacks().onEdited = [this](){
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




   */

		auto setCb = [&](Widget* w) {
			w->GetCallbacks().onEdited	  = [this]() { m_editor->GetApp()->GetGfxContext().MarkBindlessDirty(); };
			w->GetCallbacks().onEditEnded = [this]() { UpdateMaterial(); };
		};

		uint32 i = 0;

		for (MaterialProperty* p : mat->GetProperties())
		{
			if (p->propDef.type == ShaderPropertyType::Float)
			{
				Widget* layout = CommonWidgets::BuildField(m_inspector,
														   p->propDef.name,
														   p->data.data(),
														   {
															   .type	  = FieldType::Float,
															   .stepFloat = 0.1f,
															   .foldPtr	  = reinterpret_cast<bool*>(&m_propertyFoldValues[i]),
														   });
				setCb(layout);
				m_inspector->AddChild(layout);
			}
			else if (p->propDef.type == ShaderPropertyType::Int32)
			{
				Widget* layout = CommonWidgets::BuildField(m_inspector,
														   p->propDef.name,
														   p->data.data(),
														   {
															   .type	  = FieldType::Int32,
															   .stepFloat = 1.0f,
															   .foldPtr	  = reinterpret_cast<bool*>(&m_propertyFoldValues[i]),
														   });
				setCb(layout);
				m_inspector->AddChild(layout);
			}
			else if (p->propDef.type == ShaderPropertyType::UInt32)
			{
				Widget* layout = CommonWidgets::BuildField(m_inspector,
														   p->propDef.name,
														   p->data.data(),
														   {
															   .type	  = FieldType::UInt32,
															   .stepFloat = 1.0f,
															   .foldPtr	  = reinterpret_cast<bool*>(&m_propertyFoldValues[i]),
														   });
				setCb(layout);
				m_inspector->AddChild(layout);
			}
			else if (p->propDef.type == ShaderPropertyType::Bool)
			{
				Widget* layout = CommonWidgets::BuildField(m_inspector,
														   p->propDef.name,
														   p->data.data(),
														   {
															   .type	= FieldType::Boolean,
															   .foldPtr = reinterpret_cast<bool*>(&m_propertyFoldValues[i]),
														   });
				setCb(layout);
				m_inspector->AddChild(layout);
			}
			else if (p->propDef.type == ShaderPropertyType::Vec2)
			{
				Widget* layout = CommonWidgets::BuildField(m_inspector,
														   p->propDef.name,
														   p->data.data(),
														   {
															   .type	  = FieldType::Vector2,
															   .stepFloat = 0.1f,
															   .foldPtr	  = reinterpret_cast<bool*>(&m_propertyFoldValues[i]),
														   });
				setCb(layout);
				m_inspector->AddChild(layout);
			}
			else if (p->propDef.type == ShaderPropertyType::Vec3)
			{
				Widget* layout = CommonWidgets::BuildField(m_inspector,
														   p->propDef.name,
														   p->data.data(),
														   {
															   .type	  = FieldType::Vector3,
															   .stepFloat = 0.1f,
															   .foldPtr	  = reinterpret_cast<bool*>(&m_propertyFoldValues[i]),
														   });
				setCb(layout);
				m_inspector->AddChild(layout);
			}
			else if (p->propDef.type == ShaderPropertyType::Vec4)
			{
				Widget* layout = CommonWidgets::BuildField(m_inspector,
														   p->propDef.name,
														   p->data.data(),
														   {
															   .type	  = FieldType::Vector4,
															   .stepFloat = 0.1f,
															   .foldPtr	  = reinterpret_cast<bool*>(&m_propertyFoldValues[i]),
														   });

				Widget*			rightSide = layout->GetChildren().back();
				Vector<Widget*> children  = rightSide->GetChildren();
				rightSide->RemoveAllChildren();

				Button* colorButton = CommonWidgets::BuildIconButton(layout, ICON_PALETTE);
				rightSide->AddChild(colorButton);
				for (Widget* c : children)
					rightSide->AddChild(c);

				colorButton->GetProps().onClicked = [this, colorButton, p, setCb, mat, i]() {
					PanelColorWheel* pcw = m_editor->GetWindowPanelManager().OpenColorWheelPanel(colorButton);
					Color*			 col = reinterpret_cast<Color*>(p->data.data());
					pcw->GetWheel()->SetTargetColor(*col);
					pcw->SetUserData(this);

					const ResourceID matID	 = mat->GetID();
					const uint32	 propIdx = i;

					pcw->GetCallbacks().onEditEnded = [matID]() {
						Panel* panel = Editor::Get()->GetWindowPanelManager().FindPanelOfType(PanelType::MaterialViewer, matID);
						if (!panel)
							return;
						static_cast<PanelMaterialViewer*>(panel)->UpdateMaterial();
					};

					pcw->GetCallbacks().onEdited = [matID, propIdx, pcw]() {
						Material* mat = Editor::Get()->GetApp()->GetResourceManager().GetIfExists<Material>(matID);
						if (!mat)
							return;

						if (mat->GetProperties().size() <= propIdx)
							return;

						MaterialProperty* prop = mat->GetProperties().at(propIdx);

						if (prop->propDef.type != ShaderPropertyType::Vec4)
							return;

						const Color col		 = pcw->GetWheel()->GetEditedColor().SRGB2Linear();
						Color*		matColor = reinterpret_cast<Color*>(prop->data.data());
						*matColor			 = col;
						Editor::Get()->GetApp()->GetGfxContext().MarkBindlessDirty();
					};
				};

				setCb(layout);
				m_inspector->AddChild(layout);
			}
			else if (p->propDef.type == ShaderPropertyType::IVec2)
			{
				Widget* layout = CommonWidgets::BuildField(m_inspector,
														   p->propDef.name,
														   p->data.data(),
														   {
															   .type	  = FieldType::Vector2i,
															   .stepFloat = 1.0f,
															   .foldPtr	  = reinterpret_cast<bool*>(&m_propertyFoldValues[i]),
														   });
				setCb(layout);
				m_inspector->AddChild(layout);
			}
			else if (p->propDef.type == ShaderPropertyType::IVec3)
			{
				Widget* layout = CommonWidgets::BuildField(m_inspector,
														   p->propDef.name,
														   p->data.data(),
														   {
															   .type	  = FieldType::Vector3i,
															   .stepFloat = 1.0f,
															   .foldPtr	  = reinterpret_cast<bool*>(&m_propertyFoldValues[i]),
														   });
				setCb(layout);
				m_inspector->AddChild(layout);
			}
			else if (p->propDef.type == ShaderPropertyType::IVec4)
			{
				Widget* layout = CommonWidgets::BuildField(m_inspector,
														   p->propDef.name,
														   p->data.data(),
														   {
															   .type	  = FieldType::Vector4i,
															   .stepFloat = 1.0f,
															   .foldPtr	  = reinterpret_cast<bool*>(&m_propertyFoldValues[i]),
														   });
				setCb(layout);
				m_inspector->AddChild(layout);
			}
			else if (p->propDef.type == ShaderPropertyType::Matrix4)
			{
			}
			else if (p->propDef.type == ShaderPropertyType::Texture2D)
			{
				Widget* layout = CommonWidgets::BuildField(m_inspector,
														   p->propDef.name,
														   p->data.data(),
														   {
															   .type	= FieldType::UserClass,
															   .tid		= GetTypeID<LinaTexture2D>(),
															   .foldPtr = reinterpret_cast<bool*>(&m_propertyFoldValues[i]),
														   });

				setCb(layout);
				m_inspector->AddChild(layout);
			}

			i++;
		}

		m_inspector->Initialize();

		if (m_previewOnly)
			DisableRecursively(m_inspector);
	}

	void PanelMaterialViewer::UpdateMaterial()
	{
		Material* mat = static_cast<Material*>(m_resource);

		Panel* panel = m_editor->GetWindowPanelManager().FindPanelOfType(PanelType::ColorWheel, 0);
		if (panel && panel->GetUserData() == this)
		{
		}

		OStream stream;
		mat->SaveToStream(stream);
		EditorActionResourceMaterial::Create(m_editor, m_resource->GetID(), m_resourceSpace, m_previousStream, stream);
		stream.Destroy();
	}

} // namespace Lina::Editor
