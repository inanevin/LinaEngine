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

#include "Core/Graphics/GUI/GUIBackend.hpp"
#include "Core/Graphics/ResourceUploadQueue.hpp"
#include "Core/Resources/CommonResources.hpp"

namespace Lina
{
	class WorldRenderer;
	class ResourceManagerV2;
	class TextureSampler;
}; // namespace Lina

namespace LinaGX
{
	class Instance;
}

namespace Lina::Editor
{
	class Editor;
	class SurfaceRenderer;
	class EditorWorldRenderer;

	class EditorRenderer
	{
	private:
		struct WorldRendererPair
		{
			WorldRenderer*		 wr	 = nullptr;
			EditorWorldRenderer* ewr = nullptr;
		};

	public:
		void Initialize(Editor* editor);
		void Shutdown();
		void PreTick();
		void Tick(float delta);

		void SyncRender();
		void Render(uint32 frameIndex);
		void AddWorldRenderer(WorldRenderer* wr, EditorWorldRenderer* ewr);
		void RemoveWorldRenderer(WorldRenderer* wr);

		void AddSurfaceRenderer(SurfaceRenderer* sr);
		void RemoveSurfaceRenderer(SurfaceRenderer* sr);

		inline TextureSampler* GetGUISampler() const
		{
			return m_guiSampler;
		}

		inline TextureSampler* GetGUITextSampler() const
		{
			return m_guiTextSampler;
		}

		inline uint16 GetPipelineLayoutGUI()
		{
			return m_pipelineLayoutGUI;
		}

		inline const Vector<WorldRendererPair>& GetWorldRenderers()
		{
			return m_worldRenderers;
		}

	private:
		JobExecutor				  m_executor;
		Vector<Texture*>		  m_dynamicTextures;
		uint16					  m_pipelineLayoutGUI = 0;
		Vector<SurfaceRenderer*>  m_validSurfaceRenderers;
		TextureSampler*			  m_guiSampler		  = nullptr;
		TextureSampler*			  m_guiTextSampler	  = nullptr;
		ResourceManagerV2*		  m_resourceManagerV2 = nullptr;
		LinaGX::Instance*		  m_lgx				  = nullptr;
		Vector<WorldRendererPair> m_worldRenderers;
		Vector<SurfaceRenderer*>  m_surfaceRenderers;
		Editor*					  m_editor = nullptr;
	};

} // namespace Lina::Editor
