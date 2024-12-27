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
	class WorldRenderer;
	class Font;
	class Text;
	class Dropdown;
} // namespace Lina

namespace Lina::Editor
{
	class EditorCamera;
	class OrbitCamera;
	class EditorWorldRenderer;
	class Editor;

	class WorldDisplayer : public Widget, public EntityWorldListener, public EditorPayloadListener
	{
	private:
		struct GizmoControls
		{
			GizmoType	  selectedGizmo = GizmoType::Move;
			GizmoAxis	  hoveredAxis	= GizmoAxis::None;
			GizmoAxis	  pressedAxis	= GizmoAxis::None;
			GizmoLocality gizmoLocality = GizmoLocality::World;
			GizmoSnapping gizmoSnapping = GizmoSnapping::Free;
		};

		enum class DisplayTexture
		{
			WorldResult,
			WorldGBuf0,
			WorldGBuf1,
			WorldGBuf2,
			WorldDepth,
			OutlinePass,
			EntityIDPass,
		};

	public:
		struct Properties
		{
			String noWorldText		 = "";
			bool   enableDragAndDrop = false;
		};

		WorldDisplayer() : Widget(WF_CONTROLLABLE){};
		virtual ~WorldDisplayer() = default;

		virtual void			Construct() override;
		virtual void			Initialize() override;
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
		void		 DisplayWorld(WorldRenderer* renderer, EditorWorldRenderer* ewr, WorldCameraType cameraType);
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual bool OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act) override;

		// World
		virtual void OnWorldTick(float delta, PlayMode playmode) override;
		ResourceID	 GetWorldID();
		void		 SelectEntity(Entity* e, bool clearOthers);
		void		 SelectEntity(EntityID guid, bool clearOthers);
		void		 OnEntitySelectionChanged();
		void		 OnActionEntitySelection(const Vector<EntityID>& selection);

		// Gizmos
		void SelectGizmo(GizmoType gizmo);
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

	private:
		void SetDisplayTextureTitle();
		void HandleGizmoControls();
		void DestroyCamera();
		void DrawAxis(const Vector3& targetAxis, const Color& baseColor, const String& axis);

	private:
		GizmoControls m_gizmoControls;

		Dropdown*	   m_displayTextureDropdown = nullptr;
		DisplayTexture m_currentDisplayTexture	= DisplayTexture::WorldResult;

		Vector<Entity*>		 m_selectedEntities;
		Editor*				 m_editor		 = nullptr;
		EditorWorldRenderer* m_ewr			 = nullptr;
		Properties			 m_props		 = {};
		Text*				 m_noWorldText	 = nullptr;
		WorldRenderer*		 m_worldRenderer = nullptr;
		EditorCamera*		 m_camera		 = nullptr;
		Font*				 m_gizmoFont	 = nullptr;
		GUIUserData			 m_guiUserData	 = {
					   .specialType = GUISpecialType::DisplayTarget,
					   .mipLevel	= 15,
		   };
	};
	LINA_WIDGET_BEGIN(WorldDisplayer, Hidden)
	LINA_CLASS_END(WorldDisplayer)
} // namespace Lina::Editor
