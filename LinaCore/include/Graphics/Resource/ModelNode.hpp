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

#ifndef ModelNode_HPP
#define ModelNode_HPP

#include "Math/AABB.hpp"
#include "Math/Matrix.hpp"
#include "Data/Vector.hpp"
#include "Data/String.hpp"
#include "Serialization/ISerializable.hpp"

namespace Lina
{
	class Mesh;
	class ModelLoader;

	class ModelNode : public ISerializable
	{
	public:
		ModelNode() = default;
		~ModelNode();

		// Inherited via ISerializable
		virtual void SaveToStream(OStream& stream) override;
		virtual void LoadFromStream(IStream& stream) override;
		void FetchChildren(const Vector<ModelNode*>& allNodes);

		inline void ClearData()
		{
			m_name				= "";
			m_localTransform	= Matrix4();
			m_totalVertexCenter = Vector3::Zero;
			m_aabb				= AABB();
			m_children.clear();
			m_meshes.clear();
		}

		inline const Vector<Mesh*> GetMeshes()
		{
			return m_meshes;
		}

		inline AABB& GetAABB()
		{
			return m_aabb;
		}

		inline Vector3& GetVertexCenter()
		{
			return m_totalVertexCenter;
		}

		inline String& GetName()
		{
			return m_name;
		}

		inline uint32 GetNodeIndex()
		{
			return m_index;
		}

		inline Matrix4& GetLocalTransform()
		{
			return m_localTransform;
		}

		inline const Vector<ModelNode*>& GetChildren()
		{
			return m_children;
		}

	private:
		friend class ModelLoader;

		AABB			   m_aabb;
		Matrix4			   m_localTransform;
		Vector3			   m_totalVertexCenter = Vector3::Zero;
		String			   m_name			   = "";
		Vector<ModelNode*> m_children;
		Vector<uint32>	   m_childrenIndices;
		Vector<Mesh*>	   m_meshes;
		uint32			   m_index = 0;
	};
} // namespace Lina

#endif
