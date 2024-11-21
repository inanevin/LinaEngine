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

#include "Common/Data/Vector.hpp"
#include "Common/Data/String.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Common/Math/AABB.hpp"
#include "Vertex.hpp"
#include "Primitive.hpp"

namespace LinaGX
{
	class CommandStream;
}
namespace Lina
{
	class ModelNode;

	class MeshDefault
	{
	public:
		~MeshDefault();

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);

		void Draw(LinaGX::CommandStream* stream, uint32 instances);
		void Bind(LinaGX::CommandStream* stream);

		inline uint32 GetVertexOffset() const
		{
			return m_vertexOffset;
		}

		inline uint32 GetIndexOffset() const
		{
			return m_indexOffset;
		}

		inline uint32 GetIndexCount() const
		{
			return static_cast<uint32>(m_indices16.size());
		}

		inline const AABB& GetAABB() const
		{
			return m_localAABB;
		}

		inline const String& GetName() const
		{
			return m_name;
		}

		inline const Vector<PrimitiveDefault>& GetPrimitives() const
		{
			return m_primitives;
		}

		inline ModelNode* GetNode()
		{
			return m_node;
		}
		inline size_t GetSize() const
		{
			return sizeof(MeshDefault) + sizeof(PrimitiveDefault) * m_primitives.size() + sizeof(VertexDefault) * m_vertices.size() + sizeof(uint16) * m_indices16.size();
		}

	private:
		friend class Model;
		friend class ModelNode;
		friend class MeshManager;

	private:
		Vector<PrimitiveDefault> m_primitives;
		Vector<VertexDefault>	 m_vertices;
		Vector<uint16>			 m_indices16;
		ModelNode*				 m_node			= nullptr;
		String					 m_name			= "";
		uint32					 m_vertexOffset = 0;
		uint32					 m_indexOffset	= 0;
		AABB					 m_localAABB;

		Buffer m_vertexBuffer;
		Buffer m_indexBuffer;
		bool   m_ownsBuffers = false;
	};

} // namespace Lina
