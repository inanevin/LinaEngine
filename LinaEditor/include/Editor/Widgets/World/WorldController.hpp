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

namespace Lina
{
	class Dropdown;
	class WorldRenderer;
	class Font;
} // namespace Lina

namespace Lina::Editor
{
	class EditorCamera;
	class EditorWorldRenderer;

	class WorldController : public Widget, public EntityWorldListener, public EditorPayloadListener
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
			GizmoMode			   type						  = GizmoMode::Move;
			GizmoAxis			   hoveredAxis				  = GizmoAxis::None;
			GizmoAxis			   targetAxis				  = GizmoAxis::None;
			GizmoLocality		   locality					  = GizmoLocality::World;
			GizmoSnapping		   snapping					  = GizmoSnapping::Free;
			Vector3				   averagePosition			  = Vector3::Zero;
			Vector3				   pressedGizmoPosition		  = Vector3::Zero;
			Vector2				   pressedGizmoPositionScreen = Vector2::Zero;
			Vector2				   pressedMouseDelta		  = Vector2::Zero;
			Vector2				   pressedMousePosition		  = Vector2::Zero;
			Vector2				   currentMousePosition		  = Vector2::Zero;
			Vector<Transformation> pressedEntityTransforms	  = {};

			Vector3 targetAxisWorld	 = Vector3::Zero;
			Vector2 targetAxisScreen = Vector2::Zero;

			bool	visualizeLine	  = false;
			Vector2 visualizeLineP0	  = Vector2::Zero;
			Vector2 visualizeLineP1	  = Vector2::Zero;
			float	visualizeDistance = 0.0f;
			float	visualizeAlpha	  = 0.0f;
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
		void		 SetWorld(WorldRenderer* renderer, EditorWorldRenderer* ewr, WorldCameraType cameraType);
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual bool OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act) override;

		// World
		virtual void OnWorldTick(float delta, PlayMode playmode) override;
		ResourceID	 GetWorldID();
		void		 SelectEntity(Entity* e, bool clearOthers);
		void		 OnActionEntitySelection(const Vector<EntityID>& selection);
		void		 OnEntitySelectionChanged();

		// Gizmos
		void SelectGizmo(GizmoMode gizmo);
		void SelectGizmoLocality(GizmoLocality locality);
		void SelectGizmoSnap(GizmoSnapping snapping);

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
		void SetDisplayTextureTitle();
		void HandleGizmoControls();
		void DestroyCamera();
		void CalculateAverageGizmoPosition();

	private:
		WorldRenderer*		 m_worldRenderer = nullptr;
		EditorWorldRenderer* m_ewr			 = nullptr;
		EntityWorld*		 m_world		 = nullptr;
		EditorCamera*		 m_camera		 = nullptr;

		GizmoControls m_gizmoControls;

		Dropdown*	   m_displayTextureDropdown = nullptr;
		DisplayTexture m_currentDisplayTexture	= DisplayTexture::WorldFinal;

		Vector<Entity*> m_selectedEntities;
		Editor*			m_editor	= nullptr;
		Properties		m_props		= {};
		Font*			m_worldFont = nullptr;
	};

	LINA_WIDGET_BEGIN(WorldController, Hidden)
	LINA_CLASS_END(WorldController)
} // namespace Lina::Editor
