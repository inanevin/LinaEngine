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

#include "Core/World/Component.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Common/Math/AABB.hpp"

namespace Lina
{
	class MeshDefault;

	class MeshComponent : public Component
	{
	public:
		virtual void CollectReferences(HashSet<ResourceID>& refs) override
		{
			refs.insert(m_model);
			refs.insert(m_material);
		}

		virtual void SaveToStream(OStream& stream) const override
		{
			stream << m_model << m_material << m_meshIndex;
		}

		virtual void LoadFromStream(IStream& stream) override
		{
			stream >> m_model >> m_material >> m_meshIndex;
		}

		inline void SetMesh(ResourceID model, uint32 meshIndex)
		{
			m_model		= model;
			m_meshIndex = meshIndex;
		}

		inline void SetMaterial(ResourceID id)
		{
			m_material = id;
		}

		virtual TypeID GetComponentType() override
		{
			return GetTypeID<MeshComponent>();
		}

		inline ResourceID GetModel() const
		{
			return m_model;
		}

		inline ResourceID GetMaterial() const
		{
			return m_material;
		}

		inline uint32 GetMeshIndex() const
		{
			return m_meshIndex;
		}

	private:
		ResourceID m_model;
		ResourceID m_material;
		uint32	   m_meshIndex = 0;
		AABB	   m_usedLocalAABB;
	};

} // namespace Lina
