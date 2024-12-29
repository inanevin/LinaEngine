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
		m_world			= m_worldRenderer->GetWorld();

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

				m_gizmoControls.pressedGizmoPosition = m_gizmoControls.worldPosition;
				const Vector3 inScreen				 = Camera::WorldToScreen(m_world->GetWorldCamera(), m_gizmoControls.pressedGizmoPosition, GetEndFromMargins() - GetStartFromMargins());
				m_gizmoControls.pressedMouseDelta	 = (m_lgxWindow->GetMousePosition() - GetStartFromMargins()) - inScreen.XY();

				m_gizmoControls.pressedEntityPositions.reserve(m_selectedEntities.size());
				for (Entity* e : m_selectedEntities)
					m_gizmoControls.pressedEntityPositions.push_back(e->GetPosition());
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
			else
				SelectEntity(m_world->GetEntity(lastHovered), true);
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
			else
				hoveredAxis == GizmoAxis::None;

			m_gizmoControls.hoveredAxis = hoveredAxis;
		}

		const GizmoLocality locality = m_selectedEntities.size() > 1 ? GizmoLocality::World : m_gizmoControls.locality;
		rendererSettings.position	 = m_gizmoControls.worldPosition;
		rendererSettings.rotation	 = locality == GizmoLocality::World ? Quaternion::Identity() : m_selectedEntities.at(0)->GetRotation();
		rendererSettings.type		 = m_gizmoControls.type;
		rendererSettings.locality	 = locality;
		rendererSettings.hoveredAxis = m_gizmoControls.hoveredAxis;

		if (m_gizmoControls.targetAxis != GizmoAxis::None)
		{
			rendererSettings.lineVisualizeAxis = m_gizmoControls.targetAxis;

			const Camera& camera = m_world->GetWorldCamera();
			const Vector2 size	 = GetEndFromMargins() - GetStartFromMargins();
			const Vector3 pos	 = m_selectedEntities.at(0)->GetPosition();

			Vector3 targetAxisWorld = Vector3::Zero;
			Vector3 upAxisWorld		= Vector3::Up;

			Vector3 planeNormal0 = Vector3::Zero;
			Vector3 planeNormal1 = Vector3::Zero;

			if (m_gizmoControls.targetAxis == GizmoAxis::X)
			{
				targetAxisWorld = locality == GizmoLocality::World ? Vector3::Right : m_selectedEntities.at(0)->GetRotation().GetRight();
				planeNormal0	= locality == GizmoLocality::World ? Vector3::Forward : targetAxisWorld.Cross(m_selectedEntities.at(0)->GetRotation().GetUp());
				planeNormal1	= locality == GizmoLocality::World ? Vector3::Up : targetAxisWorld.Cross(m_selectedEntities.at(0)->GetRotation().GetForward());
			}
			else if (m_gizmoControls.targetAxis == GizmoAxis::Y)
			{
				targetAxisWorld = locality == GizmoLocality::World ? Vector3::Up : m_selectedEntities.at(0)->GetRotation().GetUp();
				upAxisWorld		= locality == GizmoLocality::World ? Vector3::Right : m_selectedEntities.at(0)->GetRotation().GetRight();
			}
			else if (m_gizmoControls.targetAxis == GizmoAxis::Z)
			{
				targetAxisWorld = locality == GizmoLocality::World ? Vector3::Forward : m_selectedEntities.at(0)->GetRotation().GetForward();
				upAxisWorld		= locality == GizmoLocality::World ? Vector3::Up : m_selectedEntities.at(0)->GetRotation().GetUp();
			}

			const Vector2 mp		= m_lgxWindow->GetMousePosition() - GetStartFromMargins() - m_gizmoControls.pressedMouseDelta;
			Vector4		  mouseNDC	= Vector4((mp.x / size.x) * 2.0f - 1.0f, 1.0f - (mp.y / size.y) * 2.0f, -1.0f, 1.0f);
			Vector4		  nearPoint = camera.GetInvViewProj() * mouseNDC;
			nearPoint /= nearPoint.w;
			Vector4 farPoint = camera.GetInvViewProj() * glm::vec4(mouseNDC.x, mouseNDC.y, 1.0f, 1.0f);
			farPoint /= farPoint.w;

			const Vector3 rayDirection = (farPoint.XYZ() - nearPoint.XYZ()).Normalized();
			const Vector3 rayOrigin	   = nearPoint.XYZ();

			const Vector3 planeOrigin  = m_gizmoControls.worldPosition;
			float		  hitDistance0 = 0.0f;
			float		  hitDistance1 = 0.0f;
			bool		  rayHit0	   = glm::intersectRayPlane(glm::vec3(rayOrigin), glm::vec3(rayDirection), glm::vec3(planeOrigin), glm::vec3(planeNormal0), hitDistance0);
			bool		  rayHit1	   = glm::intersectRayPlane(glm::vec3(rayOrigin), glm::vec3(rayDirection), glm::vec3(planeOrigin), glm::vec3(planeNormal1), hitDistance1);

			Vector3 hitPoint = Vector3::Zero;
			if (rayHit0 && rayHit1)
				hitPoint = rayOrigin + rayDirection * (hitDistance0 < hitDistance1 ? hitDistance0 : hitDistance1);
			else if (!rayHit0 && rayHit1)
				hitPoint = rayOrigin + rayDirection * hitDistance1;
			else if (rayHit0 && !rayHit1)
				hitPoint = rayOrigin + rayDirection * hitDistance0;
			else
			{
				// ups.
				int a = 5;
			}

			LINA_TRACE("{0}, {1}, {2}, {3}", rayHit0, hitDistance0, rayHit1, hitDistance1);
			const Vector3 dir				  = hitPoint - m_gizmoControls.pressedGizmoPosition;
			const float	  lineT				  = dir.Magnitude() * dir.Normalized().Dot(targetAxisWorld);
			const Vector3 targetWorldPosition = m_gizmoControls.pressedGizmoPosition + targetAxisWorld * lineT;

			for (Entity* e : m_selectedEntities)
				e->SetPosition(targetWorldPosition);

			// const Vector3 gizmoPosScreen		 = Camera::WorldToScreen(camera, m_gizmoControls.worldPosition, size);
			// const Vector3 gizmoPosWorldExtended	 = m_gizmoControls.worldPosition + targetAxisWorld;
			// const Vector3 gizmoPosExtendedScreen = Camera::WorldToScreen(camera, gizmoPosWorldExtended, size);
			// Vector3		  targetAxisScreen		 = (gizmoPosExtendedScreen - gizmoPosScreen);
			//
			// const Vector2 mp			   = m_lgxWindow->GetMousePosition();
			// const Vector2 screenDelta	   = (mp - m_gizmoControls.pressPosition);
			// const float	  dot			   = screenDelta.Normalized().Dot(targetAxisScreen.Normalized().XY());
			// const float	  movementInScreen = dot * screenDelta.Magnitude();
			//
			// const Vector2 targetPointInScreen = gizmoPosScreen.XY() + targetAxisScreen.XY().Normalized() * movementInScreen;
			//
			// const Vector3 planeOrigin = m_gizmoControls.worldPosition;
			// const Vector3 planeNormal = targetAxisWorld.Cross(-upAxisWorld);
			//
			// const Vector2 mouseScreen = m_lgxWindow->GetMousePosition() - GetStartFromMargins();
			// // const Vector2 mouseScreen = targetPointInScreen;
			//
			// Vector4 mouseNDC = Vector4((mouseScreen.x / size.x) * 2.0f - 1.0f, 1.0f - (mouseScreen.y / size.y) * 2.0f, -1.0f, 1.0f);
			//
			// Vector4 nearPoint = camera.GetInvViewProj() * mouseNDC;
			// nearPoint /= nearPoint.w;
			//
			// Vector4 farPoint = camera.GetInvViewProj() * glm::vec4(mouseNDC.x, mouseNDC.y, 1.0f, 1.0f);
			// farPoint /= farPoint.w;
			//
			// Vector3 rayDirection = (farPoint.XYZ() - nearPoint.XYZ()).Normalized();
			//
			// float t	  = 0.0f;
			// bool  hit = glm::intersectRayPlane(glm::vec3(nearPoint), glm::vec3(rayDirection), glm::vec3(planeOrigin), glm::vec3(planeNormal), t);
			//
			// Vector3 hp = nearPoint.XYZ() + t * rayDirection;
			//
			// const Vector3 dir		  = hp - m_gizmoControls.pressedWorldPosition;
			// const float	  lineT		  = dir.Magnitude() * dir.Normalized().Dot(targetAxisWorld);
			// rendererSettings.position = m_gizmoControls.pressedWorldPosition + targetAxisWorld * lineT;
			//
			// LINA_TRACE("{0}, {1}, {2}", lineT, hp.y, hp.z);
			//
			// //
			// const Vector3 screenHit = Camera::WorldToScreen(camera, rendererSettings.position, size);
			//
			// LinaVG::StyleOptions debug;
			// debug.color = Color::Yellow.AsLVG4();
			// m_lvg->DrawLine((m_rect.GetCenter()).AsLVG(), (targetPointInScreen).AsLVG(), debug, LinaVG::LineCapDirection::None, 0.0f, 1000);

			// glm::vec3 rayOrigin	   = glm::vec3(nearPoint);
			// glm::vec3 rayDirection = glm::normalize(glm::vec3(farPoint) - rayOrigin);
			//
			// // Plane intersection
			// float	  t					= glm::dot(glm::vec3(m_gizmoControls.pressedWorldPosition) - rayOrigin, glm::vec3(targetAxisWorld)) / glm::dot(rayDirection, glm::vec3(targetAxisWorld));
			// glm::vec3 intersectionPoint = rayOrigin + t * rayDirection;
			//
			// // Projection onto axis
			// glm::vec3 closestPointOnAxis = glm::vec3(m_gizmoControls.pressedWorldPosition) + glm::dot(intersectionPoint - glm::vec3(m_gizmoControls.pressedWorldPosition), glm::vec3(targetAxisWorld)) * glm::vec3(targetAxisWorld);
			// Vector3	  aq				 = closestPointOnAxis;
			// rendererSettings.position	 = m_gizmoControls.pressedWorldPosition + aq;
			// const Vector3 gizmoPosScreen		 = Camera::WorldToScreen(camera, m_gizmoControls.worldPosition, size);
			// const Vector3 gizmoPosWorldExtended	 = m_gizmoControls.worldPosition + targetAxisWorld;
			// const Vector3 gizmoPosExtendedScreen = Camera::WorldToScreen(camera, gizmoPosWorldExtended, size);
			// Vector3		  targetAxisScreen		 = (gizmoPosExtendedScreen - gizmoPosScreen);
			// Vector3		  ratio					 = targetAxisWorld / targetAxisScreen;
			// // const float	  ratio					 = targetAxisWorld.Magnitude() / targetAxisScreen.Magnitude();
			//
			// const Vector2 mp			   = m_lgxWindow->GetMousePosition();
			// const Vector2 screenDelta	   = (mp - m_gizmoControls.pressPosition);
			// const float	  dot			   = screenDelta.Normalized().Dot(targetAxisScreen.Normalized().XY());
			// const float	  movementInScreen = dot * screenDelta.Magnitude();
			// rendererSettings.position	   = m_gizmoControls.pressedWorldPosition + targetAxisWorld * movementInScreen * ratio.Magnitude();
			//
			// {
			// 	glm::vec2 ndcDelta = glm::vec2(2.0f * screenDelta.x / size.x, -2.0f * screenDelta.y / size.y);
			// 	glm::vec4 clipPos  = camera.GetProjection() * camera.GetView() * glm::vec4(m_gizmoControls.pressedWorldPosition, 1.0f);
			// 	clipPos /= clipPos.w;
			// 	clipPos.x += ndcDelta.x;
			// 	clipPos.y += ndcDelta.y;
			//
			// 	Vector4 worldPos = camera.GetInvViewProj() * clipPos;
			// 	worldPos /= worldPos.w;
			//
			// 	Vector3 worldDelta = worldPos.XYZ() - m_gizmoControls.pressedWorldPosition;
			//
			// 	// Project world delta onto the axis
			// 	Vector3 movement		  = worldDelta.Dot(targetAxisWorld) * targetAxisWorld;
			// 	//rendererSettings.position = m_gizmoControls.pressedWorldPosition + movement;
			// }

			// const float	  dot			   = mouseDirScreen.Normalized().Dot(targetAxisScreen.Normalized().XY());
			// const float	  movementInScreen = dot * mouseDirScreen.Magnitude();
			//
			// const Vector3 aq = Camera::ScreenToWorld(camera, Vector2(movementInScreen, 0), size, 1.0f);
			//
			// const float movementInWorld = movementInScreen * ratio;
			// rendererSettings.position	= m_gizmoControls.pressedWorldPosition + targetAxisWorld * movementInWorld;

			// for (Entity* e : m_selectedEntities)
			// {
			// 	// e->SetPosition(m_gizmoControls.gizmoPositionWorld + targetAxisWorld * movementInWorld * (dot > 0.0f ? 1.0f : -1.0f));
			// 	// e->SetPosition(e->GetPosition() + targetAxisWorld * movementInWorld * (dot > 0.0f ? 1.0f : -1.0f));
			// 	e->SetPosition(m_gizmoControls.worldPosition + targetAxisWorld * movementInWorld * (dot > 0.0f ? 1.0f : -1.0f));
			// }
			// m_gizmoControls.gizmoPositionWorld = gizmoPosInWorld;
			//  m_gizmoControls.pressPosition	   = mp;

			// m_ewr->GetGizmoRenderer().SetGizmoControls(m_gizmoControls);

			// LinaVG::StyleOptions debug;
			// debug.color = Color::Yellow.AsLVG4();
			// m_lvg->DrawLine((GetStartFromMargins() + gizmoPosScreen.XY()).AsLVG(), (GetStartFromMargins() + gizmoPosExtendedScreen.XY()).AsLVG(), debug, LinaVG::LineCapDirection::None, 0.0f, 1000);
			// debug.color = Color::Green.AsLVG4();
			// m_lvg->DrawLine((GetStartFromMargins() + gizmoPosScreen.XY()).AsLVG(), (GetStartFromMargins() + gizmoPosScreen.XY() + mouseDirScreen).AsLVG(), debug, LinaVG::LineCapDirection::None, 0.0f, 1000);
			// debug.color = Color::Cyan.AsLVG4();
			// m_lvg->DrawLine((GetStartFromMargins() + gizmoPosScreen.XY()).AsLVG(), (GetStartFromMargins() + gizmoPosScreen.XY() + targetAxisScreen.XY().Normalized() * movementInScreen).AsLVG(), debug, LinaVG::LineCapDirection::None, 0.0f, 1000);

			// const Vector3 posScreen1 = Camera::WorldToScreen(camera, pos + targetAxisWorld, size);
			// const Vector3 posBack	 = Camera::ScreenToWorld(camera, posScreen.XY(), size, posScreen.z);
			//
			//
			//
			// const Vector3 pressPosInWorld = Camera::ScreenToWorld(camera, m_gizmoControls.pressPosition, size, posScreen.z);
			// const Vector3 mouseInWorld	  = Camera::ScreenToWorld(camera, mp, size, posScreen.z);
			// const Vector3 mouseDirInWorld = mouseInWorld - pressPosInWorld;
			// const float	  mouseMagInWorld = mouseDirInWorld.Magnitude();
			//
			// const Vector2 mouseDirInScreen = mp - m_gizmoControls.pressPosition;
			// const Vector2 targetAxisScreen = (posScreen1 - posScreen).XY().Normalized();
			// const float	  dot			   = mouseDirInScreen.Normalized().Dot(targetAxisScreen);
			// const float	  mag			   = mouseDirInScreen.Magnitude();
			// const float	  movementInScreen = mag * (dot);
			// const Vector3 movementInWorld = Camera::ScreenToWorld(camera, Vector2(movementInScreen, 0), size, posScreen.z);
			// const float	  movementInWorldMag  = movementInWorld.Magnitude();

			// LINA_TRACE("movement mag in screen {0} - dot {1} - mag {2} - movement in world {3}", movementInScreen, dot, mag, movementInWorldMag);

			for (Entity* e : m_selectedEntities)
			{
				// e->SetPosition(e->GetPosition()  + targetAxisWorld * movementInWorld);
				//	e->SetPosition(e->GetPosition() + targetAxisWorld * mouseDirInWorld.Magnitude());
			}
			// const Vector2 mouseDir			   = (mp - m_gizmoControls.pressPosition);
			// const Vector3 alignmentScreenSpace = Camera::WorldToScreen(, alignment, size).Normalized();
			// float		  movement			   = mouseDir.Dot(alignmentScreenSpace.XY());

			//	LINA_TRACE("pos back {0} {1} {2}", posBack.x, posBack.y, posBack.z);

			// const Vector3 screenSpace0	 = Camera::WorldToScreen(m_world->GetWorldCamera(), pos, size);
			// const Vector3 screenSpace1	 = Camera::WorldToScreen(m_world->GetWorldCamera(), pos + alignment, size);
			// const Vector2 screenSpaceDir = (screenSpace1 - screenSpace0).XY().Normalized();
			//
			// const Vector3 wp = Camera::ScreenToWorld(m_world->GetWorldCamera(), screenSpaceDir, size, 0.9f);
			//
			// const float	  dot		  = mouseDir.Dot(screenSpaceDir);
			// const Vector3 movementDir = alignment * (dot < 0.0f ? -1.0f : 1.0f) * wp.XY().Magnitude();
			//
			// if (m_gizmoControls.selectedGizmo == GizmoType::Move)
			// {
			// 	for (Entity* e : m_selectedEntities)
			// 	{
			// 		const Vector3 epos = e->GetPosition();
			// 		const Vector3 p	   = epos + movementDir;
			//
			// 		if (!p.Equals(epos, 0.0001f))
			// 		{
			// 			int a = 5;
			// 		}
			// 		e->SetPosition(p);
			// 	}
			// }
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
			m_gizmoControls.worldPosition = Vector3::Zero;
			for (Entity* e : m_selectedEntities)
				m_gizmoControls.worldPosition += e->GetPosition();
			m_gizmoControls.worldPosition /= static_cast<float>(m_selectedEntities.size());
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
