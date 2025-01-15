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
#include "Core/Graphics/Pipeline/RenderPass.hpp"

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
	class Texture;
class Font;
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
			bool		  draw			  = false;
			bool		  drawOrientation = false;
			bool		  visualizeAxis	  = false;
			EntityID	  hoveredEntityID = 0;
			GizmoAxis	  focusedAxis	  = GizmoAxis::None;
			GizmoMode	  type			  = GizmoMode::Move;
			GizmoAxis	  hoveredAxis	  = GizmoAxis::None;
			GizmoLocality locality		  = GizmoLocality::World;
			Vector3		  position		  = Vector3::Zero;
			Quaternion	  rotation		  = Quaternion::Identity();
			GizmoSnapping snapping		  = GizmoSnapping::Free;
			Vector3		  worldAxis		  = Vector3::Zero;
			float		  angle0		  = 0.0f;
			float		  angle1		  = 0.0f;
            float defaultShaderScale = 1.0f;
		};

		GizmoRenderer(Editor* editor, WorldRenderer* wr, RenderPass* targetPass, MousePickRenderer* mpr);
		virtual ~GizmoRenderer();

		void Tick(float delta);

		inline GizmoSettings& GetSettings()
		{
			return m_gizmoSettings;
		}

        inline void SetSelectedEntities(const Vector<Entity*>& entities)
        {
            m_selectedEntities = entities;
        }
	private:
		Color GetColorFromAxis(GizmoAxis axis);
		void  DrawGizmoMoveScale(RenderPass* pass, StringID variant, float shaderScale = 1.0f);
		void  DrawGizmoRotate(RenderPass* pass, StringID variant, float shaderScale = 1.0f);
		void  DrawGizmoRotateFocus(RenderPass* pass, float shaderScale = 1.0f);
		void  DrawOrientationGizmos(RenderPass* pass, StringID variant, float shaderScale = 1.0f);
        void DrawLightIcons(RenderPass* pass, StringID variant);
        void DrawLightWireframes();
        void SetupIconMaterial(Material* mat, Font* font, uint8 codePoint, Vector2& outSize);
	private:
        Vector<Entity*> m_selectedEntities;
		Editor*			   m_editor		   = nullptr;
		WorldRenderer*	   m_worldRenderer = nullptr;
		ResourceManagerV2* m_rm			   = nullptr;
		EntityWorld*	   m_world		   = nullptr;
        Font* m_fontSDFIcon = nullptr;

		Model* m_modelTranslate	 = nullptr;
		Model* m_modelScale		 = nullptr;
		Model* m_modelRotate	 = nullptr;
		Model* m_modelRotateCircle = nullptr;
		Model* m_modelTranslateCenter = nullptr;
		Model* m_modelScaleCenter	 = nullptr;
		Model* m_modelOrientationGizmo	 = nullptr;
        Model* m_modelQuad = nullptr;

		Material*		   m_matGizmoRotateCircle = nullptr;
		Material*		   m_matGizmoCenter = nullptr;
		Material*		   m_matGizmoX		 = nullptr;
		Material*		   m_matGizmoY		 = nullptr;
		Material*		   m_matGizmoZ		 = nullptr;
        Material* m_matLightIconSun = nullptr;
        Material* m_matLightIconSpot = nullptr;
        Material* m_matLightIconPoint = nullptr;
		Shader*			   m_shaderGizmo		 = nullptr;
		Shader*			   m_shaderGizmoRotateCircle	 = nullptr;
		Shader*			   m_shaderLine		 = nullptr;
		Shader*			   m_shaderLVG			 = nullptr;
		Shader*			   m_shaderGizmoOrientation	 = nullptr;
        Shader* m_shaderSDFIcon = nullptr;
		RenderPass*		   m_targetPass			 = nullptr;
		MousePickRenderer* m_mousePickRenderer	 = nullptr;
		GizmoSettings	   m_gizmoSettings		 = {};
        
        Vector2 m_iconSzSun = Vector2::Zero;
        Vector2 m_iconSzSpot = Vector2::Zero;
        Vector2 m_iconSzPoint = Vector2::Zero;
	};
} // namespace Lina::Editor
