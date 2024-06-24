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
#include "Common/Data/String.hpp"
#include "Common/Data/Map.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"

namespace Lina
{
	class ResourceManager;
	class Shader;
	class GfxManager;
} // namespace Lina

namespace LinaGX
{
	class Instance;
}

namespace Lina::Editor
{

	struct BufferBatch
	{
		Buffer* vtxBuffer = nullptr;
		Buffer* idxBuffer = nullptr;
		size_t	vtxCount  = 0;
		size_t	idxCount  = 0;
	};

	struct LineVertex
	{
		Vector3 position;
		Vector3 nextPosition;
		Vector4 color;
		float	direction = 0.0f;
	};

	class WorldRendererExtEditor : public WorldRendererExtension
	{
	public:
		struct PerFrameData
		{
			Buffer vertexBuffer;
			Buffer indexBuffer;
		};

		WorldRendererExtEditor();
		virtual ~WorldRendererExtEditor();

		virtual void Tick(float delta) override;
		virtual void RenderForward(uint32 frameIndex, LinaGX::CommandStream* stream) override;
		virtual bool CopyBuffers(uint32 frameIndex, LinaGX::CommandStream* stream) override;

	private:
		RenderPass		  m_pass;
		GfxManager*		  m_gfxManager	= nullptr;
		LinaGX::Instance* m_lgx			= nullptr;
		ResourceManager*  m_rm			= nullptr;
		Shader*			  m_shaderLines = nullptr;
		PerFrameData	  m_pfd[FRAMES_IN_FLIGHT];
	};

} // namespace Lina::Editor