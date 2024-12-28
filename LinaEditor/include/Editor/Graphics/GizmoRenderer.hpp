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

#include "Editor/World/CommonEditorWorld.hpp"

namespace Lina
{
	class Shader;
	class Entity;
	class ResourceManagerV2;
	class WorldRenderer;
	class EntityWorld;
	class Material;
	class Model;
	class RenderPass;
} // namespace Lina

namespace LinaGX
{
	class CommandStream;
}

namespace Lina::Editor
{
	class Editor;
	class MousePickRenderer;

	class GizmoRenderer
	{
	public:
		GizmoRenderer(Editor* editor, WorldRenderer* wr, RenderPass* targetPass);
		virtual ~GizmoRenderer();

		void Tick(float delta);

		inline void SetMousePickRenderer(MousePickRenderer* mpr)
		{
			m_mpr = mpr;
		}

		inline void SetSelectedEntities(const Vector<Entity*>& entities)
		{
			m_selectedEntities = entities;
		}

		inline void SetHoveredAxis(GizmoAxis hoveredAxis)
		{
			m_hoveredAxis = hoveredAxis;
		}

		inline void SetSelectedGizmo(GizmoType gizmo)
		{
			m_selectedGizmo = gizmo;
		}

		inline void SetPressedAxis(GizmoAxis axis)
		{
			m_pressedAxis = axis;
		}

		inline void SetGizmoLocality(GizmoLocality locality)
		{
			m_gizmoLocality = locality;
		}

	private:
		void DrawGizmoMoveScale(const Vector3& pos);
		void DrawGizmoRotate(const Vector3& pos);
		void DrawGizmoAxisLine(const Vector3& pos, GizmoAxis axis, GizmoLocality locality);

	private:
		GizmoType	  m_selectedGizmo	= GizmoType::Move;
		GizmoAxis	  m_hoveredAxis		= GizmoAxis::None;
		GizmoAxis	  m_lastHoveredAxis = GizmoAxis::None;
		GizmoAxis	  m_pressedAxis		= GizmoAxis::None;
		GizmoLocality m_gizmoLocality	= GizmoLocality::World;

		MousePickRenderer* m_mpr		   = nullptr;
		Editor*			   m_editor		   = nullptr;
		WorldRenderer*	   m_worldRenderer = nullptr;
		ResourceManagerV2* m_rm			   = nullptr;
		EntityWorld*	   m_world		   = nullptr;

		Model* m_translateModel = nullptr;
		Model* m_scaleModel		= nullptr;
		Model* m_rotateModel	= nullptr;

		Vector<Entity*> m_selectedEntities = {};
		Material*		m_gizmoMaterialX   = nullptr;
		Material*		m_gizmoMaterialY   = nullptr;
		Material*		m_gizmoMaterialZ   = nullptr;
		Shader*			m_gizmoShader	   = nullptr;
		Shader*			m_line3DShader	   = nullptr;
		RenderPass*		m_targetPass	   = nullptr;
	};
} // namespace Lina::Editor
