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

namespace Lina
{
	class Shader;
	class Entity;
	class WorldRenderer;
	class EntityWorld;
	class ResourceManagerV2;
	class Material;
	class DrawCollector;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;

	class OutlineSelectionRenderer
	{
	public:
		OutlineSelectionRenderer(Editor* editor, WorldRenderer* wr);
		virtual ~OutlineSelectionRenderer();

		inline void SetSelectedEntities(const Vector<Entity*>& entities)
		{
			m_selectedEntities = entities;
		}

		void OnProduceFrame(DrawCollector& collector);
		void OnRenderPassPost(uint32 frameIndex, LinaGX::CommandStream* stream);

	private:
		Editor*			   m_editor = nullptr;
		WorldRenderer*	   m_wr		= nullptr;
		ResourceManagerV2* m_rm		= nullptr;
		EntityWorld*	   m_world	= nullptr;

		Vector<Entity*> m_selectedEntities = {};
		Material*		m_outlineMaterial  = nullptr;
		Shader*			m_outlineShader	   = nullptr;
	};
} // namespace Lina::Editor
