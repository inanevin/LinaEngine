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

#include "Common/Data/HashMap.hpp"
#include "Common/Data/Vector.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Graphics/CommonGraphics.hpp"

namespace LinaGX
{
	class Instance;
	class CommandStream;
} // namespace LinaGX

namespace Lina
{
	class Shader;
	class MeshComponent;
	class Buffer;
	class EntityWorld;
	class BindlessContext;

	class DrawCollector
	{
	public:
		void Initialize(LinaGX::Instance* lgx, EntityWorld* world, BindlessContext* context);
		void Clear();
		void CollectStaticMeshes(const Vector<MeshComponent*>& meshes, ShaderType shaderType);
		void RecordIndirectCommands(Buffer& indirectBuffer, Buffer& indirectArgsBuffer);
		void Draw(LinaGX::CommandStream* stream, Buffer& indirectBuffer);

	private:
		LinaGX::Instance*							  m_lgx	  = nullptr;
		EntityWorld*								  m_world = nullptr;
		HashMap<Shader*, Vector<DrawDataMeshDefault>> m_drawData;
		BindlessContext*							  m_bindlessContext = nullptr;
	};

} // namespace Lina
