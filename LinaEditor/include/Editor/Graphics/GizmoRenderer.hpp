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

namespace LinaVG
{
	class Drawer;
}
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
		struct GizmoSettings
		{
			bool		  draw			= false;
			bool		  visualizeAxis = false;
			GizmoAxis	  focusedAxis	= GizmoAxis::None;
			GizmoMode	  type			= GizmoMode::Move;
			GizmoAxis	  hoveredAxis	= GizmoAxis::None;
			GizmoLocality locality		= GizmoLocality::World;
			Vector3		  position		= Vector3::Zero;
			Quaternion	  rotation		= Quaternion::Identity();
			GizmoSnapping snapping		= GizmoSnapping::Free;
			Vector3		  worldAxis		= Vector3::Zero;
			float		  angle0		= 0.0f;
			float		  angle1		= 0.0f;
		};

		GizmoRenderer(Editor* editor, WorldRenderer* wr, RenderPass* targetPass, MousePickRenderer* mpr);
		virtual ~GizmoRenderer();

		void Tick(float delta);

		inline GizmoSettings& GetSettings()
		{
			return m_gizmoSettings;
		}

	private:
		Color GetColorFromAxis(GizmoAxis axis);
		void  DrawGizmoMoveScale(RenderPass* pass, StringID variant, float shaderScale = 1.0f);
		void  DrawGizmoRotate(RenderPass* pass, StringID variant, float shaderScale = 1.0f);
		void  DrawGizmoAxisLine(RenderPass* pass, GizmoAxis axis);
		void  DrawGizmoRotateFocus(RenderPass* pass, float shaderScale = 1.0f);

	private:
		Editor*			   m_editor		   = nullptr;
		WorldRenderer*	   m_worldRenderer = nullptr;
		ResourceManagerV2* m_rm			   = nullptr;
		EntityWorld*	   m_world		   = nullptr;

		Model* m_translateModel	 = nullptr;
		Model* m_scaleModel		 = nullptr;
		Model* m_rotateModel	 = nullptr;
		Model* m_rotateFullModel = nullptr;
		Model* m_centerTranslate = nullptr;
		Model* m_centerScale	 = nullptr;

		Material*		   m_gizmoRotateMaterial = nullptr;
		Material*		   m_gizmoMaterialCenter = nullptr;
		Material*		   m_gizmoMaterialX		 = nullptr;
		Material*		   m_gizmoMaterialY		 = nullptr;
		Material*		   m_gizmoMaterialZ		 = nullptr;
		Shader*			   m_gizmoShader		 = nullptr;
		Shader*			   m_gizmoRotateShader	 = nullptr;
		Shader*			   m_line3DShader		 = nullptr;
		Shader*			   m_lvgShader			 = nullptr;
		RenderPass*		   m_targetPass			 = nullptr;
		MousePickRenderer* m_mousePickRenderer	 = nullptr;
		GizmoSettings	   m_gizmoSettings		 = {};
		GizmoAxis		   m_lastHoveredAxis	 = GizmoAxis::None;
	};
} // namespace Lina::Editor
