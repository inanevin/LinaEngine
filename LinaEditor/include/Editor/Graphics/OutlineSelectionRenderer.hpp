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

#include "Core/Graphics/Renderers/FeatureRenderer.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"

namespace Lina
{
	class Shader;
	class Entity;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;

	class OutlineSelectionRenderer : public FeatureRenderer
	{
	public:
		OutlineSelectionRenderer(Editor* editor, LinaGX::Instance* lgx, EntityWorld* world, WorldRenderer* wr, ResourceManagerV2* rm);
		virtual ~OutlineSelectionRenderer();

		inline void SetSelectedEntities(const Vector<Entity*>& entities)
		{
			m_selectedEntities = entities;
		}

		virtual void OnProduceFrame(DrawCollector& collector) override;
		virtual void OnRenderPassPost(uint32 frameIndex, LinaGX::CommandStream* stream, RenderPassType type) override;

	private:
		Vector<Entity*> m_selectedEntities = {};
		Material*		m_outlineMaterial  = nullptr;
		Shader*			m_outlineShader	   = nullptr;
		Editor*			m_editor		   = nullptr;
	};
} // namespace Lina::Editor
