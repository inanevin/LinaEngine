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

#pragma once

#include "Core/GUI/Widgets/Widget.hpp"
#include "Editor/PayloadListener.hpp"
#include "Editor/World/CommonEditorWorld.hpp"
#include "Editor/Graphics/EditorGfxHelpers.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Common/Tween/Tween.hpp"

namespace Lina
{
	class Dropdown;
	class WorldRenderer;
	class Font;
	class Icon;
	class Button;
	class DirectionalLayout;
} // namespace Lina

namespace Lina::Editor
{
	class EditorCamera;
	class EditorWorldRenderer;

	class WorldController : public Widget, public EditorPayloadListener, public EntityWorldListener
	{
	public:
		enum class DisplayTexture
		{
			WorldFinal,
			WorldGBuf0,
			WorldGBuf1,
			WorldGBuf2,
			WorldDepth,
			WorldResult,
			OutlinePass,
		};

		struct GizmoControls
		{
			String				   valueStr					  = "";
			GizmoMotion			   gizmoMotion				  = GizmoMotion::None;
			GizmoMode			   type						  = GizmoMode::Move;
			GizmoAxis			   motionAxis				  = GizmoAxis::None;
			GizmoLocality		   locality					  = GizmoLocality::World;
			GizmoLocality		   usedLocality				  = GizmoLocality::World;
			GizmoSnapping		   snapping					  = GizmoSnapping::Free;
			Vector3				   averagePosition			  = Vector3::Zero;
			Vector3				   motionStartAvgPos		  = Vector3::Zero;
			Vector2				   motionStartAvgPosScreen	  = Vector2::Zero;
			Vector2				   motionStartMouseDelta	  = Vector2::Zero;
			Vector2				   motionStartMousePos		  = Vector2::Zero;
			Vector2				   motionCurrentMousePos	  = Vector2::Zero;
			Vector3				   averagePositionScreenSpace = Vector3::Zero;
			Vector<Transformation> motionStartTransforms	  = {};

			float value				= 0.0f;
			float visualizeDistance = 0.0f;
			float visualizeAlpha	= 0.0f;

			Button* buttonGizmoType = nullptr;
		};

		struct SelectionCircleButton
		{
			Button* widget = nullptr;
			Icon*	icon   = nullptr;
			float	angle  = 0.0f;
		};

		struct SelectionControls
		{
			bool						  isParenting = false;
			Tween						  circleTween = {};
			float						  endAngle	  = 0.0f;
			bool						  visible	  = false;
			Vector<SelectionCircleButton> buttons;
			float						  _radius = 0.0f;

			Button* parentButton				= nullptr;
			Button* localityButton				= nullptr;
			bool	rectSelectionPressed		= false;
			bool	rectSelectionWaitingResults = false;
			Vector2 rectSelectionStartPosition	= Vector2::Zero;
		};

		struct OverlayControls
		{
			Widget*			   baseWidget		   = nullptr;
			DirectionalLayout* topToolbar		   = nullptr;
			Button*			   buttonSnapOptions   = nullptr;
			Button*			   buttonWorldOptions  = nullptr;
			Button*			   buttonCameraOptions = nullptr;

			SnappingOptions snappingOptions = {};
			CameraOptions	cameraOptions	= {};

			SnappingOptions		 oldSnappingOptions = {};
			CameraOptions		 oldCameraOptions	= {};
			WorldGfxSettings	 oldGfxSettings		= {};
			WorldPhysicsSettings oldPhysicsOptions	= {};
		};

	public:
		struct Properties
		{
			String noWorldText		 = "";
			bool   enableDragAndDrop = true;
		};

		WorldController() : Widget(WF_CONTROLLABLE){};
		virtual ~WorldController() = default;

		virtual void			Construct() override;
		virtual void			Destruct() override;
		virtual void			PreTick() override;
		virtual void			Tick(float dt) override;
		virtual void			Draw() override;
		virtual void			OnPayloadStarted(PayloadType type, Widget* payload) override;
		virtual void			OnPayloadEnded(PayloadType type, Widget* payload) override;
		virtual bool			OnPayloadDropped(PayloadType type, Widget* payload) override;
		virtual LinaGX::Window* OnPayloadGetWindow() override
		{
			return m_lgxWindow;
		}
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual bool OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act) override;
		void		 SetWorld(WorldRenderer* renderer, EditorWorldRenderer* ewr, WorldCameraType cameraType);
		void		 FocusSelected();

		// World
		virtual void OnWorldTick(float delta, PlayMode playmode) override;
		ResourceID	 GetWorldID();
		void		 OnEntitySelectionChanged(const Vector<Entity*>& selection);
		void		 StartPlaying(PlayMode mode);
		void		 StopPlaying();

		// Gizmos
		void SelectGizmo(GizmoMode gizmo);
		void SelectGizmoLocality(GizmoLocality locality);
		void SelectGizmoSnap(GizmoSnapping snapping);

		// World
		void SetCameraOptions(const CameraOptions& opts);
		void SetSnappingOptions(const SnappingOptions& opts);

		inline EditorCamera* GetWorldCamera()
		{
			return m_camera;
		}

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline DisplayTexture GetCurrentDisplayTexture() const
		{
			return m_currentDisplayTexture;
		}

	private:
		void BuildOverlayItems();
		void BuildSelectionCircle();
		void BuildSnappingOptions();
		void BuildCameraOptions();
		void BuildWorldOptions();

		void DuplicateSelection();
		void DeleteSelection();
		void StartSelectionParenting();
		void EndSelectionParenting(bool apply);
		void SetDisplayTextureTitle();
		void HandleGizmoControls();
		void DestroyCamera();
		void CalculateAverageGizmoPosition();

		void StartGizmoMotion(GizmoMotion motion, GizmoAxis axis);
		void StopGizmoMotion(bool apply);

	private:
		WorldRenderer*		 m_worldRenderer = nullptr;
		EditorWorldRenderer* m_ewr			 = nullptr;
		EntityWorld*		 m_world		 = nullptr;
		EditorCamera*		 m_camera		 = nullptr;

		GizmoControls m_gizmoControls;

		Dropdown*	   m_displayTextureDropdown = nullptr;
		DisplayTexture m_currentDisplayTexture	= DisplayTexture::WorldFinal;

		Vector<Entity*>	  m_selectedEntities;
		Vector<Entity*>	  m_selectedRoots;
		Editor*			  m_editor			  = nullptr;
		Properties		  m_props			  = {};
		Font*			  m_worldFont		  = nullptr;
		SelectionControls m_selectionControls = {};
		OverlayControls	  m_overlayControls	  = {};
		PlayMode		  m_playMode		  = PlayMode::None;
		OStream			  m_savedWorld;
	};

	LINA_WIDGET_BEGIN(WorldController, Hidden)
	LINA_CLASS_END(WorldController)
} // namespace Lina::Editor
