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

#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/World/EntityWorld.hpp"

namespace LinaGX
{
	class CommandStream;
	class Instance;
} // namespace LinaGX

namespace Lina
{
	class GUIBackend;
	class GfxManager;
	class ModelNode;
	class ModelComponent;
	class Shader;

	class WorldRenderer : public EntityWorldListener
	{
	private:
		struct PerFrameData
		{
			LinaGX::CommandStream* gfxStream		 = nullptr;
			LinaGX::CommandStream* copyStream		 = nullptr;
			SemaphoreData		   copySemaphore	 = {};
			Buffer				   guiVertexBuffer	 = {};
			Buffer				   guiIndexBuffer	 = {};
			Buffer				   guiMaterialBuffer = {};

			uint32 colorTarget = 0;
			uint32 depthTarget = 0;
		};

	public:
		WorldRenderer(GfxManager* man, EntityWorld* world, const Vector2ui& viewSize);
		~WorldRenderer();

		void				   Tick(float delta);
		LinaGX::CommandStream* Render(uint32 frameIndex, int32 threadIndex);

		virtual void OnComponentAdded(Component* c) override;
		virtual void OnComponentRemoved(Component* c) override;

		inline const SemaphoreData& GetCopySemaphore(uint32 index) const
		{
			return m_pfd[index].copySemaphore;
		}

		inline EntityWorld* GetWorld() const
		{
			return m_world;
		}

	private:
		void FetchRenderables();
		void DrawSky(LinaGX::CommandStream* stream);

	private:
		GfxManager*				m_gfxManager			= nullptr;
		GUIBackend*				m_guiBackend			= nullptr;
		LinaGX::Instance*		m_lgx					= nullptr;
		PerFrameData			m_pfd[FRAMES_IN_FLIGHT] = {};
		RenderPass				m_mainPass				= {};
		Vector2ui				m_size					= Vector2ui::Zero;
		EntityWorld*			m_world					= nullptr;
		ModelNode*				m_skyCube				= nullptr;
		Shader*					m_skyShader				= nullptr;
		Vector<ModelComponent*> m_modelComponents;
		GPUDataView				m_gpuDataView  = {};
		GPUDataScene			m_gpuDataScene = {};
	};

} // namespace Lina
