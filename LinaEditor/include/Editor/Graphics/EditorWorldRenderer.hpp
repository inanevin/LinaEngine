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

#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/Renderers/WorldRendererListener.hpp"
#include "Editor/Graphics/GizmoRenderer.hpp"
#include "Editor/Graphics/MousePickRenderer.hpp"
#include "Editor/Graphics/OutlineSelectionRenderer.hpp"

namespace Lina
{
	class Shader;
	class Entity;
	class WorldRenderer;
	class ResourceManagerV2;
	class EntityWorld;
	class TextureSampler;
} // namespace Lina

namespace LinaGX
{
	class Instance;
	class CommandStream;
} // namespace LinaGX

namespace Lina::Editor
{
	class Editor;

	class EditorWorldRenderer : WorldRendererListener
	{
	private:
		struct PerFrameData
		{
			SemaphoreData copySemaphore	  = {};
			SemaphoreData signalSemaphore = {};

			Texture* renderTargetMSAA	 = nullptr;
			Texture* renderTargetResolve = nullptr;
			Texture* depthTarget		 = nullptr;
		};

	public:
		EditorWorldRenderer(Editor* editor, LinaGX::Instance* lgx, WorldRenderer* wr);
		virtual ~EditorWorldRenderer();

		void Tick(float delta);
		void SyncRender();
		void UpdateBuffers(uint32 frameIndex);
		void Render(uint32 frameIndex);

		virtual void OnWorldRendererCreateSizeRelative() override;
		virtual void OnWorldRendererDestroySizeRelative() override;

		void SetSelectedEntities(const Vector<Entity*>& selected);

		inline SemaphoreData GetSubmitSemaphore(uint32 frameIndex)
		{
			return m_pfd[frameIndex].signalSemaphore;
		};

		inline MousePickRenderer& GetMousePick()
		{
			return m_mousePickRenderer;
		}

		inline GizmoRenderer& GetGizmoRenderer()
		{
			return m_gizmoRenderer;
		}

		inline Texture* GetRenderTarget(uint32 frameIndex) const
		{
			return m_pfd[frameIndex].renderTargetResolve;
		}

		inline OutlineSelectionRenderer& GetOutlineRenderer()
		{
			return m_outlineRenderer;
		}

	private:
	private:
		Vector<Entity*>			 m_selectedEntities;
		EntityWorld*			 m_world = nullptr;
		RenderPass				 m_pass;
		LinaGX::Instance*		 m_lgx			  = nullptr;
		WorldRenderer*			 m_wr			  = nullptr;
		ResourceManagerV2*		 m_rm			  = nullptr;
		Editor*					 m_editor		  = nullptr;
		uint16					 m_pipelineLayout = 0;
		PerFrameData			 m_pfd[FRAMES_IN_FLIGHT];
		MousePickRenderer		 m_mousePickRenderer;
		OutlineSelectionRenderer m_outlineRenderer;
		GizmoRenderer			 m_gizmoRenderer;

		Shader*			m_gridShader			 = nullptr;
		Shader*			m_worldSampleShader		 = nullptr;
		Shader*			m_worldDepthSampleShader = nullptr;
		Material*		m_gridMaterial			 = nullptr;
		TextureSampler* m_worldSampler			 = nullptr;
		Vector2ui		m_size					 = Vector2ui::Zero;
	};
} // namespace Lina::Editor
