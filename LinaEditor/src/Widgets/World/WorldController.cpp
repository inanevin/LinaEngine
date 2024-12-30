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

#include "Editor/Widgets/World/WorldController.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/Panel/PanelResourceBrowser.hpp"
#include "Editor/Widgets/Compound/ResourceDirectoryBrowser.hpp"
#include "Editor/Graphics/EditorWorldRenderer.hpp"
#include "Editor/World/EditorCamera.hpp"
#include "Editor/World/WorldUtility.hpp"
#include "Editor/Actions/EditorActionEntity.hpp"

#include "Core/GUI/Widgets/Layout/Popup.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Application.hpp"
#include "Common/Math/Math.hpp"
#include "Common/System/SystemInfo.hpp"
#include <LinaGX/Core/InputMappings.hpp>
#include <glm/gtx/intersect.hpp>

namespace Lina::Editor
{
	void WorldController::Construct()
	{
		m_editor = Editor::Get();

		m_worldFont = m_editor->GetApp()->GetResourceManager().GetResource<Font>(EDITOR_FONT_PLAY_BIG_ID);

		DirectionalLayout* topToolbar = m_manager->Allocate<DirectionalLayout>("TopToolbar");
		topToolbar->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_X_TOTAL_CHILDREN | WF_USE_FIXED_SIZE_Y);
		topToolbar->SetAlignedPos(Vector2::Zero);
		topToolbar->SetAlignedSizeX(1.0f);
		topToolbar->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		topToolbar->SetFixedSizeX(Theme::GetDef().baseItemHeight);
		topToolbar->GetWidgetProps().childPadding		= Theme::GetDef().baseIndent;
		topToolbar->GetWidgetProps().drawOrderIncrement = 1;
		AddChild(topToolbar);

		m_displayTextureDropdown = m_manager->Allocate<Dropdown>("DisplayTextureDD");
		m_displayTextureDropdown->GetFlags().Set(WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_SIZE_ALIGN_Y);
		m_displayTextureDropdown->SetAlignedPosY(0.0f);
		m_displayTextureDropdown->SetAlignedSizeY(1.0f);
		m_displayTextureDropdown->SetFixedSizeX(Theme::GetDef().baseItemHeight * 8);
		topToolbar->AddChild(m_displayTextureDropdown);

		SetDisplayTextureTitle();
		m_displayTextureDropdown->GetProps().onAddItems = [this](Popup* popup) {
			popup->AddToggleItem("Final Composition", m_currentDisplayTexture == DisplayTexture::WorldFinal, 0);
			popup->AddToggleItem("GBuffer 0", m_currentDisplayTexture == DisplayTexture::WorldGBuf0, 1);
			popup->AddToggleItem("GBuffer 1", m_currentDisplayTexture == DisplayTexture::WorldGBuf1, 2);
			popup->AddToggleItem("GBuffer 2", m_currentDisplayTexture == DisplayTexture::WorldGBuf2, 3);
			popup->AddToggleItem("GBuffer 3", m_currentDisplayTexture == DisplayTexture::WorldDepth, 4);
			popup->AddToggleItem("World Output", m_currentDisplayTexture == DisplayTexture::WorldResult, 5);
			popup->AddToggleItem("Editor Outline", m_currentDisplayTexture == DisplayTexture::OutlinePass, 6);
		};

		m_displayTextureDropdown->GetProps().onSelected = [this](int32 idx, String& outTitle) -> bool {
			m_currentDisplayTexture = static_cast<DisplayTexture>(idx);
			SetDisplayTextureTitle();
			return false;
		};

		m_editor->GetWindowPanelManager().AddPayloadListener(this);
		m_editor->GetWorldManager().AddWorldController(this);

		m_gizmoControls.type	 = static_cast<GizmoMode>(m_editor->GetSettings().GetParams().GetParamUint8("GizmoType"_hs, 0));
		m_gizmoControls.locality = static_cast<GizmoLocality>(m_editor->GetSettings().GetParams().GetParamUint8("GizmoLocality"_hs, 0));
		m_gizmoControls.snapping = static_cast<GizmoSnapping>(m_editor->GetSettings().GetParams().GetParamUint8("GizmoSnapping"_hs, 0));
	}

	void WorldController::Destruct()
	{
		m_editor->GetWindowPanelManager().RemovePayloadListener(this);
		m_editor->GetWorldManager().RemoveWorldController(this);

		if (m_worldRenderer)
			m_world->RemoveListener(this);

		DestroyCamera();
	}

	void WorldController::SetWorld(WorldRenderer* renderer, EditorWorldRenderer* ewr, WorldCameraType cameraType)
	{
		DestroyCamera();
		m_worldRenderer = renderer;
		m_ewr			= ewr;
		m_world			= renderer ? m_worldRenderer->GetWorld() : nullptr;

		if (m_worldRenderer)
		{
			m_world->AddListener(this);

			if (cameraType == WorldCameraType::Orbit)
				m_camera = new OrbitCamera(m_world);
			else if (cameraType == WorldCameraType::FreeMove)
				m_camera = new FreeCamera(m_world);
		}
	}

	void WorldController::PreTick()
	{
		if (m_worldRenderer == nullptr)
			return;

		// Input setup
		const bool worldHasFocus = m_manager->GetControlsOwner() == this && m_lgxWindow->HasFocus();
		m_world->GetInput().SetIsActive(worldHasFocus);
		m_world->GetInput().SetWheelActive(m_isHovered && m_lgxWindow->HasFocus());

		HandleGizmoControls();
	}

	void WorldController::Tick(float dt)
	{
		if (m_worldRenderer == nullptr)
			return;
	}

	void WorldController::Draw()
	{
		if (m_worldRenderer == nullptr)
			return;

		if (m_gizmoControls.type == GizmoMode::Move)
		{
			LinaVG::StyleOptions style;
			style.thickness = 1.0f;
			style.aaEnabled = true;

			const Vector2 point = Camera::WorldToScreen(m_world->GetWorldCamera(), m_gizmoControls.averagePosition, GetEndFromMargins() - GetStartFromMargins()).XY();
			// m_lvg->DrawCircle((GetStartFromMargins() + point).AsLVG(), 8.0f, style, 64, 0.0f, 0.0f, 360.0f, m_drawOrder + 1);
		}

		if (m_gizmoControls.visualizeLine)
		{
			LinaVG::StyleOptions style;
			style.thickness		= 2.0f;
			style.aaMultiplier	= 1.0f;
			style.aaEnabled		= true;
			style.color.start.w = style.color.end.w = m_gizmoControls.visualizeAlpha;

			if (m_gizmoControls.targetAxis == GizmoAxis::X)
				style.color = Theme::GetDef().accentPrimary2.AsLVG4();
			else if (m_gizmoControls.targetAxis == GizmoAxis::Y)
				style.color = Theme::GetDef().accentSuccess.AsLVG4();
			else if (m_gizmoControls.targetAxis == GizmoAxis::Z)
				style.color = Theme::GetDef().accentSecondary.AsLVG4();

			const Vector2 l0	  = GetStartFromMargins() + m_gizmoControls.visualizeLineP0;
			const Vector2 l1	  = GetStartFromMargins() + m_gizmoControls.visualizeLineP1;
			const Vector2 dir	  = (l1 - l0).Normalized();
			const Vector2 rotated = Vector2(dir.y, -dir.x);

			const Vector2 l01 = l0 + rotated * 100;

			// m_lvg->DrawLine(l0.AsLVG(), l1.AsLVG(), style, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder + 1);

			// LinaVG::TextOptions txt;
			// txt.font		  = m_worldFont->GetFont(m_lgxWindow->GetDPIScale());
			// txt.color		  = Theme::GetDef().foreground0.AsLVG4();
			// txt.color.start.w = txt.color.end.w = m_gizmoControls.visualizeAlpha;
			// const String str					= UtilStr::FloatToString(m_gizmoControls.visualizeDistance, 3);
			// m_lvg->DrawTextDefault(str.c_str(), ((l0 + l1) * 0.5f).AsLVG(), txt, 0.0f, m_drawOrder + 1, true);
		}
	}

	void WorldController::OnPayloadStarted(PayloadType type, Widget* payload)
	{
		if (!m_props.enableDragAndDrop || type != PayloadType::Resource || m_worldRenderer == nullptr)
			return;
	}

	void WorldController::OnPayloadEnded(PayloadType type, Widget* payload)
	{
		if (!m_props.enableDragAndDrop || type != PayloadType::Resource || m_worldRenderer == nullptr)
			return;
	}

	bool WorldController::OnPayloadDropped(PayloadType type, Widget* payload)
	{
		if (!m_props.enableDragAndDrop || type != PayloadType::Resource || m_worldRenderer == nullptr)
			return false;
		if (!m_isHovered)
			return false;

		Panel* panel = m_editor->GetWindowPanelManager().FindPanelOfType(PanelType::ResourceBrowser, 0);

		if (panel == nullptr)
			return false;

		const Vector<ResourceDirectory*>& payloadItems = static_cast<PanelResourceBrowser*>(panel)->GetBrowser()->GetPayloadItems();

		HashSet<ResourceID> resources;

		Entity* last = nullptr;

		EntityWorld* world = m_world;

		for (ResourceDirectory* dir : payloadItems)
		{
			if (dir->resourceTID == GetTypeID<Model>())
			{
				WorldUtility::LoadModelAndMaterials(m_editor, dir->resourceID, world->GetID());

				Model*	model  = m_resourceManager->GetResource<Model>(dir->resourceID);
				Entity* entity = WorldUtility::AddModelToWorld(world, model, model->GetMeta().materials);
				world->LoadMissingResources(*m_resourceManager, m_editor->GetProjectManager().GetProjectData(), {}, world->GetID());

				const Vector2 mp	= m_lgxWindow->GetMousePosition() - GetStartFromMargins();
				const Vector2 size	= GetEndFromMargins() - GetStartFromMargins();
				const Vector3 point = Camera::ScreenToWorld(world->GetWorldCamera(), mp, size, 0.97f);
				entity->SetPosition(point);

				last = entity;
			}
		}

		SelectEntity(last, true);
		return true;
	}

	bool WorldController::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (m_worldRenderer == nullptr)
			return false;

		if (m_isHovered)
			m_manager->GrabControls(this);
		else
			m_manager->ReleaseControls(this);

		for (Widget* c : m_children)
		{
			if (m_manager->PassMouse(c, button, act))
				return true;
		}

		if (m_isHovered && button == LINAGX_MOUSE_0 && (act == LinaGX::InputAction::Pressed))
		{
			const EntityID lastHovered = m_ewr->GetMousePick().GetLastHoveredEntity();

			auto update = [&]() {
				CalculateAverageGizmoPosition();

				m_gizmoControls.pressedGizmoPosition	   = m_gizmoControls.averagePosition;
				const Vector3 inScreen					   = Camera::WorldToScreen(m_world->GetWorldCamera(), m_gizmoControls.pressedGizmoPosition, GetEndFromMargins() - GetStartFromMargins());
				m_gizmoControls.pressedGizmoPositionScreen = GetStartFromMargins() + inScreen.XY();
				m_gizmoControls.pressedMouseDelta		   = (m_lgxWindow->GetMousePosition() - GetStartFromMargins()) - inScreen.XY();
				m_gizmoControls.pressedMousePosition	   = m_lgxWindow->GetMousePosition();

				const size_t sz = m_selectedEntities.size();
				m_gizmoControls.pressedEntityTransforms.resize(sz);
				for (size_t i = 0; i < sz; i++)
					m_gizmoControls.pressedEntityTransforms[i] = m_selectedEntities.at(i)->GetTransform();
			};

			if (lastHovered == GIZMO_GUID_X_AXIS)
			{
				m_gizmoControls.targetAxis = GizmoAxis::X;
				update();
			}
			else if (lastHovered == GIZMO_GUID_Y_AXIS)
			{
				m_gizmoControls.targetAxis = GizmoAxis::Y;
				update();
			}
			else if (lastHovered == GIZMO_GUID_Z_AXIS)
			{
				m_gizmoControls.targetAxis = GizmoAxis::Z;
				update();
			}
			else if (lastHovered == GIZMO_GUID_CENTER_AXIS)
			{
				m_gizmoControls.targetAxis = GizmoAxis::Center;
				update();
			}
			else
				SelectEntity(m_world->GetEntity(lastHovered), !m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LSHIFT));
		}

		return false;
	}

	bool WorldController::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act)
	{
		if (m_worldRenderer == nullptr)
			return false;

		if (m_manager->GetControlsOwner() != this)
			return false;

		if (keycode == LINAGX_KEY_1 && act == LinaGX::InputAction::Pressed)
		{
			SelectGizmo(GizmoMode::Move);
			return true;
		}

		if (keycode == LINAGX_KEY_2 && act == LinaGX::InputAction::Pressed)
		{
			SelectGizmo(GizmoMode::Rotate);
			return true;
		}

		if (keycode == LINAGX_KEY_3 && act == LinaGX::InputAction::Pressed)
		{
			SelectGizmo(GizmoMode::Scale);
			return true;
		}

		if (keycode == LINAGX_KEY_4 && act == LinaGX::InputAction::Pressed)
		{
			if (m_gizmoControls.locality == GizmoLocality::Local)
				SelectGizmoLocality(GizmoLocality::World);
			else
				SelectGizmoLocality(GizmoLocality::Local);

			return true;
		}

		if (keycode == LINAGX_KEY_5 && act == LinaGX::InputAction::Pressed)
		{
			const uint8 current = static_cast<uint8>(m_gizmoControls.snapping);
			const uint8 target	= (current + 1) % 3;
			SelectGizmoSnap(static_cast<GizmoSnapping>(target));
			return true;
		}

		return true;
	}

	void WorldController::SelectEntity(Entity* e, bool clearOthers)
	{
		const Vector<Entity*> prev = m_selectedEntities;

		if (clearOthers)
			m_selectedEntities.clear();

		if (e != nullptr)
			m_selectedEntities.push_back(e);

		EditorActionEntitySelection::Create(m_editor, m_world->GetID(), prev, m_selectedEntities);
		OnEntitySelectionChanged();
	}

	void WorldController::OnActionEntitySelection(const Vector<EntityID>& selection)
	{
		m_selectedEntities.clear();
		for (EntityID guid : selection)
			m_selectedEntities.push_back(m_world->GetEntity(guid));
		OnEntitySelectionChanged();
	}

	void WorldController::OnEntitySelectionChanged()
	{
		m_ewr->SetSelectedEntities(m_selectedEntities);
	}

	void WorldController::SelectGizmo(GizmoMode gizmo)
	{
		m_gizmoControls.type = gizmo;
		m_editor->GetSettings().GetParams().SetParamUint8("GizmoType"_hs, static_cast<uint8>(gizmo));
		m_editor->SaveSettings();
	}

	void WorldController::SelectGizmoLocality(GizmoLocality locality)
	{
		// Can't on multiselect
		if (locality == GizmoLocality::Local && m_selectedEntities.size() > 1)
			locality = GizmoLocality::World;

		m_gizmoControls.locality = locality;
		m_editor->GetSettings().GetParams().SetParamUint8("GizmoLocality"_hs, static_cast<uint8>(locality));
		m_editor->SaveSettings();
	}

	void WorldController::SelectGizmoSnap(GizmoSnapping snapping)
	{
		m_gizmoControls.snapping = snapping;
		m_editor->GetSettings().GetParams().SetParamUint8("GizmoSnapping"_hs, static_cast<uint8>(snapping));
		m_editor->SaveSettings();
	}

	ResourceID WorldController::GetWorldID()
	{
		return m_worldRenderer == nullptr ? 0 : m_world->GetID();
	}

	void WorldController::SetDisplayTextureTitle()
	{
		if (m_currentDisplayTexture == DisplayTexture::WorldFinal)
			m_displayTextureDropdown->GetText()->UpdateTextAndCalcSize("Final Composition");
		else if (m_currentDisplayTexture == DisplayTexture::WorldGBuf0)
			m_displayTextureDropdown->GetText()->UpdateTextAndCalcSize("GBuffer 0");
		else if (m_currentDisplayTexture == DisplayTexture::WorldGBuf1)
			m_displayTextureDropdown->GetText()->UpdateTextAndCalcSize("GBuffer 1");
		else if (m_currentDisplayTexture == DisplayTexture::WorldGBuf2)
			m_displayTextureDropdown->GetText()->UpdateTextAndCalcSize("GBuffer 2");
		else if (m_currentDisplayTexture == DisplayTexture::WorldDepth)
			m_displayTextureDropdown->GetText()->UpdateTextAndCalcSize("GBuffer Depth");
		else if (m_currentDisplayTexture == DisplayTexture::WorldResult)
			m_displayTextureDropdown->GetText()->UpdateTextAndCalcSize("World Output");
		else if (m_currentDisplayTexture == DisplayTexture::OutlinePass)
			m_displayTextureDropdown->GetText()->UpdateTextAndCalcSize("Editor Outline");
	}

	void WorldController::HandleGizmoControls()
	{
		GizmoRenderer::GizmoSettings& rendererSettings = m_ewr->GetGizmoRenderer().GetSettings();

		rendererSettings.draw = !m_selectedEntities.empty();
		if (!rendererSettings.draw)
			return;

		if (m_gizmoControls.targetAxis != GizmoAxis::None && !m_lgxWindow->GetInput()->GetMouseButton(LINAGX_MOUSE_0))
			m_gizmoControls.targetAxis = GizmoAxis::None;

		CalculateAverageGizmoPosition();

		// Hovered state.
		{
			const EntityID hovered	   = m_ewr->GetMousePick().GetLastHoveredEntity();
			GizmoAxis	   hoveredAxis = GizmoAxis::None;

			if (m_gizmoControls.targetAxis != GizmoAxis::None)
				hoveredAxis = m_gizmoControls.targetAxis;
			else if (hovered == GIZMO_GUID_X_AXIS)
				hoveredAxis = GizmoAxis::X;
			else if (hovered == GIZMO_GUID_Y_AXIS)
				hoveredAxis = GizmoAxis::Y;
			else if (hovered == GIZMO_GUID_Z_AXIS)
				hoveredAxis = GizmoAxis::Z;
			else if (hovered == GIZMO_GUID_CENTER_AXIS)
				hoveredAxis = GizmoAxis::Center;
			else
				hoveredAxis == GizmoAxis::None;

			m_gizmoControls.hoveredAxis = hoveredAxis;
		}

		if (m_gizmoControls.type == GizmoMode::Scale)
			rendererSettings.rotation = m_selectedEntities.size() == 1 ? m_selectedEntities.at(0)->GetRotation() : Quaternion::Identity();
		else
			rendererSettings.rotation = (m_selectedEntities.size() > 1 || m_gizmoControls.locality == GizmoLocality::World) ? Quaternion::Identity() : m_selectedEntities.at(0)->GetRotation();

		rendererSettings.position	 = m_gizmoControls.averagePosition;
		rendererSettings.type		 = m_gizmoControls.type;
		rendererSettings.hoveredAxis = m_gizmoControls.hoveredAxis;
		rendererSettings.focusedAxis = m_gizmoControls.targetAxis;

		m_gizmoControls.visualizeLine  = m_gizmoControls.targetAxis != GizmoAxis::None;
		m_gizmoControls.visualizeAlpha = Math::Lerp(m_gizmoControls.visualizeAlpha, m_gizmoControls.visualizeLine ? 1.0f : 0.0f, SystemInfo::GetDeltaTime() * 10.0f);

		if (m_gizmoControls.targetAxis == GizmoAxis::None)
			return;

		auto ray = [this](const Vector2& mp, const Vector3& planeNormal, Vector3& hitPoint, float& hitDistance) -> bool {
			const Camera& camera = m_world->GetWorldCamera();
			const Vector2 size	 = GetEndFromMargins() - GetStartFromMargins();

			Vector4 mouseNDC  = Vector4((mp.x / size.x) * 2.0f - 1.0f, 1.0f - (mp.y / size.y) * 2.0f, -1.0f, 1.0f);
			Vector4 nearPoint = camera.GetInvViewProj() * mouseNDC;
			nearPoint /= nearPoint.w;
			Vector4 farPoint = camera.GetInvViewProj() * glm::vec4(mouseNDC.x, mouseNDC.y, 1.0f, 1.0f);
			farPoint /= farPoint.w;

			const Vector3 rayDirection = (farPoint.XYZ() - nearPoint.XYZ()).Normalized();
			const Vector3 rayOrigin	   = nearPoint.XYZ();

			const Vector3 planeOrigin = m_gizmoControls.averagePosition;

			bool hit = glm::intersectRayPlane(glm::vec3(rayOrigin), glm::vec3(rayDirection), glm::vec3(planeOrigin), glm::vec3(planeNormal), hitDistance);
			hitPoint = rayOrigin + rayDirection * hitDistance;
			return hit;
		};

		if (m_gizmoControls.type == GizmoMode::Move)
		{
			Vector3		  targetAxisWorld = Vector3::Zero;
			const Camera& camera		  = m_world->GetWorldCamera();

			Vector3 hitPoint0 = Vector3::Zero, hitPoint1 = Vector3::Zero;
			bool	rayHit0 = false, rayHit1 = false;
			float	hitDistance0 = 0.0f, hitDistance1 = 0.0f;

			const GizmoLocality locality = m_selectedEntities.size() > 1 ? GizmoLocality::World : m_gizmoControls.locality;
			const Vector2		mp		 = m_lgxWindow->GetMousePosition() - GetStartFromMargins() - m_gizmoControls.pressedMouseDelta;

			bool isCenter = false;
			if (m_gizmoControls.targetAxis == GizmoAxis::X)
			{
				targetAxisWorld		  = locality == GizmoLocality::World ? Vector3::Right : m_selectedEntities.at(0)->GetRotation().GetRight();
				const Vector3 normal0 = locality == GizmoLocality::World ? Vector3::Forward : targetAxisWorld.Cross(m_selectedEntities.at(0)->GetRotation().GetUp());
				const Vector3 normal1 = locality == GizmoLocality::World ? Vector3::Up : targetAxisWorld.Cross(m_selectedEntities.at(0)->GetRotation().GetForward());
				rayHit0				  = ray(mp, normal0, hitPoint0, hitDistance0);
				rayHit1				  = ray(mp, normal1, hitPoint1, hitDistance1);
			}
			else if (m_gizmoControls.targetAxis == GizmoAxis::Y)
			{
				targetAxisWorld		  = locality == GizmoLocality::World ? Vector3::Up : m_selectedEntities.at(0)->GetRotation().GetUp();
				const Vector3 normal0 = locality == GizmoLocality::World ? Vector3::Forward : targetAxisWorld.Cross(m_selectedEntities.at(0)->GetRotation().GetUp());
				const Vector3 normal1 = locality == GizmoLocality::World ? Vector3::Right : targetAxisWorld.Cross(m_selectedEntities.at(0)->GetRotation().GetForward());
				rayHit0				  = ray(mp, normal0, hitPoint0, hitDistance0);
				rayHit1				  = ray(mp, normal1, hitPoint1, hitDistance1);
			}
			else if (m_gizmoControls.targetAxis == GizmoAxis::Z)
			{
				targetAxisWorld		  = locality == GizmoLocality::World ? Vector3::Forward : m_selectedEntities.at(0)->GetRotation().GetForward();
				const Vector3 normal0 = locality == GizmoLocality::World ? Vector3::Up : targetAxisWorld.Cross(m_selectedEntities.at(0)->GetRotation().GetUp());
				const Vector3 normal1 = locality == GizmoLocality::World ? Vector3::Right : targetAxisWorld.Cross(m_selectedEntities.at(0)->GetRotation().GetRight());
				rayHit0				  = ray(mp, normal0, hitPoint0, hitDistance0);
				rayHit1				  = ray(mp, normal1, hitPoint1, hitDistance1);
			}
			else if (m_gizmoControls.targetAxis == GizmoAxis::Center)
			{
				const Vector3 normal0 = (camera.GetPosition() - m_gizmoControls.averagePosition).Normalized();
				rayHit0				  = ray(mp, normal0, hitPoint0, hitDistance0);
				isCenter			  = true;
			}

			Vector3 hitPoint = Vector3::Zero;
			if (rayHit0 && rayHit1)
				hitPoint = hitDistance0 < hitDistance1 ? hitPoint0 : hitPoint1;
			else if (!rayHit0 && rayHit1)
				hitPoint = hitPoint1;
			else if (rayHit0 && !rayHit1)
				hitPoint = hitPoint0;
			else
			{
			}

			const Vector3 dir	= hitPoint - m_gizmoControls.pressedGizmoPosition;
			const float	  lineT = isCenter ? (dir.Magnitude()) : dir.Magnitude() * dir.Normalized().Dot(targetAxisWorld);
			const size_t  sz	= m_selectedEntities.size();
			const Vector3 axis	= isCenter ? dir.Normalized() : targetAxisWorld;
			for (size_t i = 0; i < sz; i++)
			{
				Entity*				  e		 = m_selectedEntities.at(i);
				const Transformation& stored = m_gizmoControls.pressedEntityTransforms.at(i);
				e->SetPosition(stored.GetPosition() + axis * lineT);
			}
		}
		else if (m_gizmoControls.type == GizmoMode::Scale)
		{
			const Camera& camera = m_world->GetWorldCamera();

			Vector3		  scaleAxis = Vector3::Zero;
			const Vector2 mp		= m_lgxWindow->GetMousePosition() - GetStartFromMargins() - m_gizmoControls.pressedMouseDelta;

			auto getScaleAmt = [&](const Vector3& targetAxis) -> float {
				Vector3		  hitPoint0	   = Vector3::Zero;
				bool		  rayHit0	   = false;
				float		  hitDistance0 = false;
				const Vector3 normal0	   = (camera.GetPosition() - m_gizmoControls.averagePosition).Normalized();
				ray(mp, normal0, hitPoint0, hitDistance0);
				const Vector3 dir = hitPoint0 - m_gizmoControls.averagePosition;
				const float	  dot = dir.Normalized().Dot(targetAxis);
				return dir.Magnitude() * (dot > 0.0f ? 1.0f : -1.0f);
			};

			float scaleAmt = 0.0f;

			if (m_gizmoControls.targetAxis == GizmoAxis::X)
			{
				const Vector3 ta = m_selectedEntities.size() > 1 ? Vector3::Right : m_selectedEntities.at(0)->GetRotation().GetRight();
				scaleAmt		 = getScaleAmt(ta);
				scaleAxis		 = Vector3::Right;
			}
			else if (m_gizmoControls.targetAxis == GizmoAxis::Y)
			{
				const Vector3 ta = m_selectedEntities.size() > 1 ? Vector3::Up : m_selectedEntities.at(0)->GetRotation().GetUp();
				scaleAmt		 = getScaleAmt(ta);
				scaleAxis		 = Vector3::Up;
			}
			else if (m_gizmoControls.targetAxis == GizmoAxis::Z)
			{
				const Vector3 ta = m_selectedEntities.size() > 1 ? Vector3::Forward : m_selectedEntities.at(0)->GetRotation().GetForward();
				scaleAmt		 = getScaleAmt(ta);
				scaleAxis		 = Vector3::Forward;
			}
			else if (m_gizmoControls.targetAxis == GizmoAxis::Center)
			{
				const Vector2 mouseDir = (m_lgxWindow->GetMousePosition() - m_gizmoControls.pressedMousePosition);
				scaleAmt			   = mouseDir.Normalized().Dot(Vector2(1, -1)) * mouseDir.Magnitude() * 0.005f;
				scaleAxis			   = Vector3::One;
			}

			const size_t sz = m_selectedEntities.size();

			for (size_t i = 0; i < sz; i++)
			{
				Entity*				  e		 = m_selectedEntities.at(i);
				const Transformation& stored = m_gizmoControls.pressedEntityTransforms.at(i);
				e->SetScale(stored.GetScale() + scaleAxis * scaleAmt);
			}
		}
		else if (m_gizmoControls.type == GizmoMode::Rotate)
		{
			const GizmoLocality locality	 = m_selectedEntities.size() > 1 ? GizmoLocality::World : m_gizmoControls.locality;
			const Camera&		camera		 = m_world->GetWorldCamera();
			Vector3				hitPoint0	 = Vector3::Zero;
			bool				rayHit0		 = false;
			float				hitDistance0 = 0.0f;

			const Vector2 pressedDir = m_gizmoControls.pressedMousePosition - m_gizmoControls.pressedGizmoPositionScreen;
			const Vector2 mouseDir	 = m_lgxWindow->GetMousePosition() - m_gizmoControls.pressedGizmoPositionScreen;
			float		  deltaAngle = mouseDir.Normalized().Angle(pressedDir.Normalized());
			const Vector3 worldDir	 = camera.GetPosition() - m_gizmoControls.averagePosition;

			if (deltaAngle > 90)
				deltaAngle = 0;

			Vector3 axis = Vector3::Zero;

			if (m_gizmoControls.targetAxis == GizmoAxis::X)
			{
				axis			= locality == GizmoLocality::World ? Vector3::Right : m_selectedEntities.at(0)->GetRotation().GetRight();
				const float dot = worldDir.Normalized().Dot(rendererSettings.rotation.GetRight());
				deltaAngle *= dot > 0.0f ? -1.0f : 1.0f;
			}
			else if (m_gizmoControls.targetAxis == GizmoAxis::Y)
			{
				axis			= locality == GizmoLocality::World ? Vector3::Up : m_selectedEntities.at(0)->GetRotation().GetUp();
				const float dot = worldDir.Normalized().Dot(rendererSettings.rotation.GetUp());
				deltaAngle *= dot > 0.0f ? -1.0f : 1.0f;
			}
			else if (m_gizmoControls.targetAxis == GizmoAxis::Z)
			{
				axis			= locality == GizmoLocality::World ? Vector3::Forward : m_selectedEntities.at(0)->GetRotation().GetForward();
				const float dot = worldDir.Normalized().Dot(rendererSettings.rotation.GetForward());
				deltaAngle *= dot > 0.0f ? -1.0f : 1.0f;
			}

			m_gizmoControls.pressedMousePosition = m_lgxWindow->GetMousePosition();

			const size_t sz = m_selectedEntities.size();

			// magic 	Vector3			 local = e->GetRotation().Inverse() * axis;

			for (size_t i = 0; i < sz; i++)
			{
				Entity*			 e	 = m_selectedEntities.at(i);
				const Quaternion rot = Quaternion::AngleAxis(deltaAngle, axis);
				e->SetRotation(rot * e->GetRotation());
			}
		}
	}

	void WorldController::DestroyCamera()
	{
		if (m_camera == nullptr)
			return;
		delete m_camera;
		m_camera = nullptr;
	}

	void WorldController::CalculateAverageGizmoPosition()
	{
		if (!m_selectedEntities.empty())
		{
			m_gizmoControls.averagePosition = Vector3::Zero;
			for (Entity* e : m_selectedEntities)
				m_gizmoControls.averagePosition += e->GetPosition();
			m_gizmoControls.averagePosition /= static_cast<float>(m_selectedEntities.size());
		}
	}

	void WorldController::OnWorldTick(float delta, PlayMode playmode)
	{
		if (m_camera)
			m_camera->Tick(delta);
	}

	/*
	void WorldController::DrawAxis(const Vector3& targetAxis, const Color& baseColor, const String& axis)
	{
		EntityWorld*  world	   = m_world;
		const Camera& worldCam = world->GetWorldCamera();

		const float length		 = 40;
		const float circleRadius = 10.0f;
		const int	lineDO		 = m_drawOrder + 1;
		const int	circleDO	 = lineDO + 1;
		const int	textDO		 = circleDO + 1;

		const Vector2 displaySize	 = world->GetScreen().GetDisplaySize();
		const Vector2 axesGizmoStart = Vector2(m_rect.pos.x + Theme::GetDef().baseIndent * 6, m_rect.GetEnd().y - Theme::GetDef().baseIndent * 6);
		const Vector3 front			 = worldCam.GetPosition() + worldCam.GetRotation().GetForward() * 1.0f;

		const Vector3 target   = targetAxis * worldCam.GetZFar();
		const float	  dot	   = Math::Abs(worldCam.GetRotation().GetForward().Dot(targetAxis));
		const Vector2 screen0  = Camera::WorldToScreen(worldCam, front, displaySize).XY();
		const Vector2 screen1  = Camera::WorldToScreen(worldCam, target, displaySize).XY();
		const Vector2 dir	   = (screen1 - screen0).Normalized();
		const Vector2 line0	   = axesGizmoStart;
		const Vector2 line1	   = line0 + dir * (1.0f - dot) * length;
		const Vector2 line1Neg = line0 - dir * (1.0f - dot) * length;

		const float overallAlpha = Math::Remap((line1 - line0).Magnitude(), 0.0f, length, 0.1f, 1.0f);

		LinaVG::StyleOptions lineStyle;
		lineStyle.thickness = Theme::GetDef().baseOutlineThickness;
		lineStyle.aaEnabled = true;
		lineStyle.color		= baseColor.AsLVG4();

		LinaVG::StyleOptions circleStyle;
		circleStyle.thickness					 = Theme::GetDef().baseOutlineThickness;
		circleStyle.outlineOptions.thickness	 = Theme::GetDef().baseOutlineThickness;
		circleStyle.outlineOptions.drawDirection = LinaVG::OutlineDrawDirection::Inwards;
		circleStyle.outlineOptions.color		 = baseColor.AsLVG4();
		circleStyle.color						 = baseColor.AsLVG4();
		circleStyle.color.start.w = circleStyle.color.end.w = 0.5f * overallAlpha;
		circleStyle.outlineOptions.color.start.w = circleStyle.outlineOptions.color.end.w = overallAlpha;

		LinaVG::TextOptions gizmoText;
		gizmoText.font			= m_gizmoFont->GetFont(m_lgxWindow->GetDPIScale());
		gizmoText.color			= Theme::GetDef().foreground0.AsLVG4();
		gizmoText.color.start.w = gizmoText.color.end.w = overallAlpha;

		m_lvg->DrawLine(line0.AsLVG(), line1.AsLVG(), lineStyle, LinaVG::LineCapDirection::None, 0.0f, lineDO);
		m_lvg->DrawCircle(line1.AsLVG(), circleRadius, circleStyle, 36, 0.0f, 0.0f, 360.0f, circleDO);

		m_lvg->DrawCircle(line1Neg.AsLVG(), circleRadius, circleStyle, 36, 0.0f, 0.0f, 360.0f, circleDO);

		const Vector2 sz = m_lvg->CalculateTextSize(axis.c_str(), gizmoText);
		m_lvg->DrawTextDefault(axis.c_str(), (line1 + Vector2(-sz.x * 0.5f, sz.y * 0.4f)).AsLVG(), gizmoText, 0.0f, textDO, false);
	}
	*/
} // namespace Lina::Editor
