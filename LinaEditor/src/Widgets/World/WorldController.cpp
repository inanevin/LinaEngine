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

		if (m_gizmoControls.gizmoMotion == GizmoMotion::Key)
		{
			LinaVG::StyleOptions style;
			style.thickness = 1.0f;
			style.aaEnabled = true;

			const Vector2 point = Camera::WorldToScreen(m_world->GetWorldCamera(), m_gizmoControls.averagePosition, GetEndFromMargins() - GetStartFromMargins()).XY();

			LinaVG::TextOptions opts;
			opts.font		   = m_worldFont->GetFont(m_lgxWindow->GetDPIScale());
			opts.color		   = Theme::GetDef().foreground0.AsLVG4();
			opts.color.start.w = opts.color.end.w = m_gizmoControls.visualizeAlpha;

			const Vector2 pos	= m_lgxWindow->GetMousePosition();
			const String  value = UtilStr::FloatToString(m_gizmoControls.visualizeDistance, 3);
			m_lvg->DrawTextDefault(value.c_str(), pos.AsLVG(), opts, 0.0f, m_drawOrder + 1);
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

			if (lastHovered == GIZMO_GUID_X_AXIS)
			{
				StartGizmoMotion(GizmoMotion::Mouse, GizmoAxis::X);
			}
			else if (lastHovered == GIZMO_GUID_Y_AXIS)
			{
				StartGizmoMotion(GizmoMotion::Mouse, GizmoAxis::Y);
			}
			else if (lastHovered == GIZMO_GUID_Z_AXIS)
			{
				StartGizmoMotion(GizmoMotion::Mouse, GizmoAxis::Z);
			}
			else if (lastHovered == GIZMO_GUID_CENTER_AXIS)
			{
				StartGizmoMotion(GizmoMotion::Mouse, GizmoAxis::Center);
			}
			else
			{
				Entity* entity = m_world->GetEntity(lastHovered);

				if (m_gizmoControls.gizmoMotion != GizmoMotion::None)
					StopGizmoMotion();

				SelectEntity(entity, !m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LSHIFT));
			}
		}

		return false;
	}

	bool WorldController::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act)
	{
		if (m_worldRenderer == nullptr)
			return false;

		if (m_manager->GetControlsOwner() != this)
			return false;

		if (act != LinaGX::InputAction::Pressed)
			return false;

		if (keycode == LINAGX_KEY_X && m_gizmoControls.gizmoMotion == GizmoMotion::None)
		{
			StartGizmoMotion(GizmoMotion::Key, GizmoAxis::X);
			return true;
		}

		if (keycode == LINAGX_KEY_Y && m_gizmoControls.gizmoMotion == GizmoMotion::None)
		{
			StartGizmoMotion(GizmoMotion::Key, GizmoAxis::Y);
			return true;
		}

		if (keycode == LINAGX_KEY_Z && m_gizmoControls.gizmoMotion == GizmoMotion::None)
		{
			StartGizmoMotion(GizmoMotion::Key, GizmoAxis::Z);
			return true;
		}

		if (keycode == LINAGX_KEY_E)
		{
			if (m_gizmoControls.gizmoMotion != GizmoMotion::None)
				StopGizmoMotion();
			SelectGizmo(GizmoMode::Move);
			return true;
		}

		if (keycode == LINAGX_KEY_R)
		{
			if (m_gizmoControls.gizmoMotion != GizmoMotion::None)
				StopGizmoMotion();
			SelectGizmo(GizmoMode::Rotate);
			return true;
		}

		if (keycode == LINAGX_KEY_T)
		{
			if (m_gizmoControls.gizmoMotion != GizmoMotion::None)
				StopGizmoMotion();
			SelectGizmo(GizmoMode::Scale);
			return true;
		}

		if (keycode == LINAGX_KEY_Q)
		{
			if (m_gizmoControls.locality == GizmoLocality::Local)
				SelectGizmoLocality(GizmoLocality::World);
			else
				SelectGizmoLocality(GizmoLocality::Local);

			return true;
		}

		if (keycode == LINAGX_KEY_Z)
		{
			const uint8 current = static_cast<uint8>(m_gizmoControls.snapping);
			const uint8 target	= (current + 1) % 3;
			SelectGizmoSnap(static_cast<GizmoSnapping>(target));
			return true;
		}

		if (m_gizmoControls.gizmoMotion == GizmoMotion::Key)
		{

			if (keycode == LINAGX_KEY_BACKSPACE)
			{
				if (!m_gizmoControls.valueStr.empty())
					m_gizmoControls.valueStr.erase(m_gizmoControls.valueStr.length() - 1, 1);

				uint32 outDecimals	  = 0;
				m_gizmoControls.value = UtilStr::StringToFloat(m_gizmoControls.valueStr, outDecimals);
				LINA_TRACE("{0}", m_gizmoControls.value);

				return true;
			}
			uint16	mask	   = 0;
			wchar_t wcharacter = L' ';
			m_lgxWindow->GetInput()->GetCharacterInfoFromKeycode(keycode, wcharacter, mask);
			uint16 characterMask = LinaGX::CharacterMask::Number | LinaGX::CharacterMask::Separator | LinaGX::CharacterMask::Sign;
			if (Bitmask<uint16>(mask).IsSet(characterMask))
			{
				const String insert = UtilStr::WCharToString(wcharacter);
				m_gizmoControls.valueStr.insert(m_gizmoControls.valueStr.size(), insert);

				uint32 outDecimals	  = 0;
				m_gizmoControls.value = UtilStr::StringToFloat(m_gizmoControls.valueStr, outDecimals);
				LINA_TRACE("{0}", m_gizmoControls.value);
				return true;
			}
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
		if (m_gizmoControls.gizmoMotion != GizmoMotion::None)
			StopGizmoMotion();

		m_selectedEntities.clear();
		for (EntityID guid : selection)
			m_selectedEntities.push_back(m_world->GetEntity(guid));
		OnEntitySelectionChanged();
	}

	void WorldController::OnActionEntityTransforms(const Vector<EntityID>& entities, const Vector<Transformation>& transforms)
	{
		size_t idx = 0;
		for (EntityID guid : entities)
		{
			Entity* e = m_world->GetEntity(guid);

			if (e != nullptr)
				e->SetTransform(transforms.at(idx));
			idx++;
		}
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
		rendererSettings.focusedAxis				   = GizmoAxis::None;
		rendererSettings.hoveredAxis				   = GizmoAxis::None;
		rendererSettings.draw						   = false;

		if (m_selectedEntities.empty())
			return;

		if (m_gizmoControls.gizmoMotion == GizmoMotion::Mouse && !m_lgxWindow->GetInput()->GetMouseButton(LINAGX_MOUSE_0))
		{
			StopGizmoMotion();
		}

		CalculateAverageGizmoPosition();

		// Hovered state.
		{
			const EntityID hovered	   = m_ewr->GetMousePick().GetLastHoveredEntity();
			GizmoAxis	   hoveredAxis = GizmoAxis::None;

			if (m_gizmoControls.motionAxis != GizmoAxis::None)
				hoveredAxis = m_gizmoControls.motionAxis;
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
			rendererSettings.hoveredAxis = hoveredAxis;
		}

		rendererSettings.draw		   = true;
		rendererSettings.visualizeAxis = false;
		rendererSettings.position	   = m_gizmoControls.averagePosition;
		rendererSettings.type		   = m_gizmoControls.type;
		rendererSettings.focusedAxis   = m_gizmoControls.motionAxis;

		if (m_gizmoControls.type == GizmoMode::Scale)
			rendererSettings.rotation = m_selectedEntities.size() == 1 ? m_selectedEntities.at(0)->GetRotation() : Quaternion::Identity();
		else
			rendererSettings.rotation = (m_selectedEntities.size() > 1 || m_gizmoControls.locality == GizmoLocality::World) ? Quaternion::Identity() : m_selectedEntities.at(0)->GetRotation();

		m_gizmoControls.visualizeAlpha = Math::Lerp(m_gizmoControls.visualizeAlpha, m_gizmoControls.gizmoMotion == GizmoMotion::Key ? 1.0f : 0.0f, SystemInfo::GetDeltaTime() * 10.0f);

		if (m_gizmoControls.motionAxis == GizmoAxis::None)
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
			const Vector2		mp		 = m_lgxWindow->GetMousePosition() - GetStartFromMargins() - m_gizmoControls.motionStartMouseDelta;

			bool isCenter = false;
			if (m_gizmoControls.motionAxis == GizmoAxis::X)
			{
				targetAxisWorld		  = locality == GizmoLocality::World ? Vector3::Right : m_selectedEntities.at(0)->GetRotation().GetRight();
				const Vector3 normal0 = locality == GizmoLocality::World ? Vector3::Forward : targetAxisWorld.Cross(m_selectedEntities.at(0)->GetRotation().GetUp());
				const Vector3 normal1 = locality == GizmoLocality::World ? Vector3::Up : targetAxisWorld.Cross(m_selectedEntities.at(0)->GetRotation().GetForward());
				rayHit0				  = ray(mp, normal0, hitPoint0, hitDistance0);
				rayHit1				  = ray(mp, normal1, hitPoint1, hitDistance1);
			}
			else if (m_gizmoControls.motionAxis == GizmoAxis::Y)
			{
				targetAxisWorld		  = locality == GizmoLocality::World ? Vector3::Up : m_selectedEntities.at(0)->GetRotation().GetUp();
				const Vector3 normal0 = locality == GizmoLocality::World ? Vector3::Forward : targetAxisWorld.Cross(m_selectedEntities.at(0)->GetRotation().GetUp());
				const Vector3 normal1 = locality == GizmoLocality::World ? Vector3::Right : targetAxisWorld.Cross(m_selectedEntities.at(0)->GetRotation().GetForward());
				rayHit0				  = ray(mp, normal0, hitPoint0, hitDistance0);
				rayHit1				  = ray(mp, normal1, hitPoint1, hitDistance1);
			}
			else if (m_gizmoControls.motionAxis == GizmoAxis::Z)
			{
				targetAxisWorld		  = locality == GizmoLocality::World ? Vector3::Forward : m_selectedEntities.at(0)->GetRotation().GetForward();
				const Vector3 normal0 = locality == GizmoLocality::World ? Vector3::Up : targetAxisWorld.Cross(m_selectedEntities.at(0)->GetRotation().GetUp());
				const Vector3 normal1 = locality == GizmoLocality::World ? Vector3::Right : targetAxisWorld.Cross(m_selectedEntities.at(0)->GetRotation().GetRight());
				rayHit0				  = ray(mp, normal0, hitPoint0, hitDistance0);
				rayHit1				  = ray(mp, normal1, hitPoint1, hitDistance1);
			}
			else if (m_gizmoControls.motionAxis == GizmoAxis::Center)
			{
				const Vector3 normal0 = (camera.GetPosition() - m_gizmoControls.averagePosition).Normalized();
				rayHit0				  = ray(mp, normal0, hitPoint0, hitDistance0);
				isCenter			  = true;
			}

			rendererSettings.visualizeAxis = true;
			rendererSettings.worldAxis	   = targetAxisWorld;

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

			const Vector3 dir	= hitPoint - m_gizmoControls.motionStartAvgPos;
			float		  lineT = isCenter ? (dir.Magnitude()) : dir.Magnitude() * dir.Normalized().Dot(targetAxisWorld);
			const size_t  sz	= m_selectedEntities.size();
			const Vector3 axis	= isCenter ? dir.Normalized() : targetAxisWorld;

			if (m_gizmoControls.gizmoMotion == GizmoMotion::Key && !m_gizmoControls.valueStr.empty())
				lineT = m_gizmoControls.value;

			m_gizmoControls.visualizeDistance = lineT;
			for (size_t i = 0; i < sz; i++)
			{
				Entity*				  e		 = m_selectedEntities.at(i);
				const Transformation& stored = m_gizmoControls.motionStartTransforms.at(i);
				e->SetPosition(stored.GetPosition() + axis * lineT);
			}
		}
		else if (m_gizmoControls.type == GizmoMode::Scale)
		{
			const Camera& camera = m_world->GetWorldCamera();

			Vector3		  scaleAxis = Vector3::Zero;
			const Vector2 mp		= m_lgxWindow->GetMousePosition() - GetStartFromMargins() - m_gizmoControls.motionStartMouseDelta;

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

			rendererSettings.visualizeAxis = true;

			if (m_gizmoControls.motionAxis == GizmoAxis::X)
			{
				const Vector3 ta		   = m_selectedEntities.size() > 1 ? Vector3::Right : m_selectedEntities.at(0)->GetRotation().GetRight();
				rendererSettings.worldAxis = ta;

				scaleAmt  = getScaleAmt(ta);
				scaleAxis = Vector3::Right;
			}
			else if (m_gizmoControls.motionAxis == GizmoAxis::Y)
			{
				const Vector3 ta		   = m_selectedEntities.size() > 1 ? Vector3::Up : m_selectedEntities.at(0)->GetRotation().GetUp();
				rendererSettings.worldAxis = ta;

				scaleAmt  = getScaleAmt(ta);
				scaleAxis = Vector3::Up;
			}
			else if (m_gizmoControls.motionAxis == GizmoAxis::Z)
			{
				const Vector3 ta		   = m_selectedEntities.size() > 1 ? Vector3::Forward : m_selectedEntities.at(0)->GetRotation().GetForward();
				rendererSettings.worldAxis = ta;

				scaleAmt  = getScaleAmt(ta);
				scaleAxis = Vector3::Forward;
			}
			else if (m_gizmoControls.motionAxis == GizmoAxis::Center)
			{
				const Vector2 mouseDir		   = (m_lgxWindow->GetMousePosition() - m_gizmoControls.motionStartMousePos);
				scaleAmt					   = mouseDir.Normalized().Dot(Vector2(1, -1)) * mouseDir.Magnitude() * 0.005f;
				scaleAxis					   = Vector3::One;
				rendererSettings.visualizeAxis = false;
			}

			const size_t sz = m_selectedEntities.size();

			if (m_gizmoControls.gizmoMotion == GizmoMotion::Key && !m_gizmoControls.valueStr.empty())
				scaleAmt = m_gizmoControls.value;
			m_gizmoControls.visualizeDistance = scaleAmt;

			for (size_t i = 0; i < sz; i++)
			{
				Entity*				  e		 = m_selectedEntities.at(i);
				const Transformation& stored = m_gizmoControls.motionStartTransforms.at(i);
				e->SetScale(stored.GetScale() + scaleAxis * scaleAmt);
			}
		}
		else if (m_gizmoControls.type == GizmoMode::Rotate)
		{
			const GizmoLocality locality = m_selectedEntities.size() > 1 ? GizmoLocality::World : m_gizmoControls.locality;
			const Camera&		camera	 = m_world->GetWorldCamera();

			const Vector2 pressedDir = (m_gizmoControls.motionStartMousePos - m_gizmoControls.motionStartAvgPosScreen).Normalized();
			const Vector2 currentDir = (m_gizmoControls.motionCurrentMousePos - m_gizmoControls.motionStartAvgPosScreen).Normalized();
			const Vector2 mouseDir	 = (m_lgxWindow->GetMousePosition() - m_gizmoControls.motionStartAvgPosScreen).Normalized();
			float		  deltaAngle = mouseDir.Angle(currentDir);
			const Vector3 worldDir	 = camera.GetPosition() - m_gizmoControls.averagePosition;

			if (deltaAngle > 90)
				deltaAngle = 0;

			Vector3 axis = Vector3::Zero;

			if (m_gizmoControls.motionAxis == GizmoAxis::X)
			{
				axis			= locality == GizmoLocality::World ? Vector3::Right : m_selectedEntities.at(0)->GetRotation().GetRight();
				const float dot = worldDir.Normalized().Dot(rendererSettings.rotation.GetRight());
				deltaAngle *= dot > 0.0f ? -1.0f : 1.0f;
			}
			else if (m_gizmoControls.motionAxis == GizmoAxis::Y)
			{
				axis			= locality == GizmoLocality::World ? Vector3::Up : m_selectedEntities.at(0)->GetRotation().GetUp();
				const float dot = worldDir.Normalized().Dot(rendererSettings.rotation.GetUp());
				deltaAngle *= dot > 0.0f ? -1.0f : 1.0f;
			}
			else if (m_gizmoControls.motionAxis == GizmoAxis::Z)
			{
				axis			= locality == GizmoLocality::World ? Vector3::Forward : m_selectedEntities.at(0)->GetRotation().GetForward();
				const float dot = worldDir.Normalized().Dot(rendererSettings.rotation.GetForward());
				deltaAngle *= dot > 0.0f ? -1.0f : 1.0f;
			}

			rendererSettings.visualizeAxis = true;
			rendererSettings.worldAxis	   = axis;

			const bool isKeyMode = m_gizmoControls.gizmoMotion == GizmoMotion::Key && !m_gizmoControls.valueStr.empty();

			// Compute signed angle (counterclockwise is positive, clockwise is negative)
			float angle0 = atan2(pressedDir.y, pressedDir.x);
			float angle1 = atan2(currentDir.y, currentDir.x);

			if (angle0 < 0)
				angle0 += 2.0f * MATH_PI;
			if (angle1 < 0)
				angle1 += 2.0f * MATH_PI;

			if (isKeyMode)
			{
				angle0 = 0;
				angle1 = DEG_2_RAD * m_gizmoControls.value;
			}

			// Send these to the shader
			rendererSettings.angle0				  = angle0;
			rendererSettings.angle1				  = angle1;
			m_gizmoControls.motionCurrentMousePos = m_lgxWindow->GetMousePosition();

			const size_t sz = m_selectedEntities.size();

			// magic 	Vector3			 local = e->GetRotation().Inverse() * axis;

			if (isKeyMode)
				deltaAngle = m_gizmoControls.value;

			m_gizmoControls.visualizeDistance = deltaAngle;

			for (size_t i = 0; i < sz; i++)
			{
				Entity*			 e	 = m_selectedEntities.at(i);
				const Quaternion rot = Quaternion::AngleAxis(deltaAngle, axis);

				if (isKeyMode)
					e->SetRotation(rot * m_gizmoControls.motionStartTransforms.at(i).GetRotation());
				else
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

	void WorldController::StartGizmoMotion(GizmoMotion motion, GizmoAxis axis)
	{
		m_gizmoControls.value = 0.0f;
		m_gizmoControls.valueStr.clear();

		CalculateAverageGizmoPosition();
		m_gizmoControls.gizmoMotion				= motion;
		m_gizmoControls.motionAxis				= axis;
		m_gizmoControls.motionStartAvgPos		= m_gizmoControls.averagePosition;
		const Vector3 inScreen					= Camera::WorldToScreen(m_world->GetWorldCamera(), m_gizmoControls.motionStartAvgPos, GetEndFromMargins() - GetStartFromMargins());
		m_gizmoControls.motionStartAvgPosScreen = GetStartFromMargins() + inScreen.XY();
		m_gizmoControls.motionStartMouseDelta	= (m_lgxWindow->GetMousePosition() - GetStartFromMargins()) - inScreen.XY();
		m_gizmoControls.motionStartMousePos		= m_lgxWindow->GetMousePosition();
		m_gizmoControls.motionCurrentMousePos	= m_lgxWindow->GetMousePosition();

		const size_t sz = m_selectedEntities.size();
		m_gizmoControls.motionStartTransforms.resize(sz);
		for (size_t i = 0; i < sz; i++)
			m_gizmoControls.motionStartTransforms[i] = m_selectedEntities.at(i)->GetTransform();
	}

	void WorldController::StopGizmoMotion()
	{
		m_gizmoControls.motionAxis	= GizmoAxis::None;
		m_gizmoControls.gizmoMotion = GizmoMotion::None;
		EditorActionEntityTransform::Create(m_editor, m_world->GetID(), m_selectedEntities, m_gizmoControls.motionStartTransforms);
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
