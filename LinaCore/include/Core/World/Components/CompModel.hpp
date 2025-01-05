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

#include "Common/Math/Transformation.hpp"
#include "Core/World/Component.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Common/Math/AABB.hpp"
#include "Core/World/Animation/AnimationController.hpp"

namespace Lina
{
	struct CompModelNode
	{
		Transformation transform;
		Matrix4		   inverseBindTransform;
		int32		   parentIndex = -1;
	};

	class CompModel : public Component
	{
	public:
		CompModel() : Component(GetTypeID<CompModel>(), 0), m_animationController(this){};

		void		 SetModel(Model* model);
		Matrix4		 CalculateGlobalMatrix(int32 nodeIndex);
		virtual void StoreReferences() override;

		virtual void CollectReferences(HashSet<ResourceID>& refs) override
		{
			refs.insert(m_model);

			for (ResourceID id : m_materials)
				refs.insert(id);
		}

		virtual void SaveToStream(OStream& stream) const override
		{
			stream << m_model << m_materials;
		}

		virtual void LoadFromStream(IStream& stream) override
		{
			stream >> m_model >> m_materials;
		}

		inline void SetMaterial(ResourceID material, uint32 index)
		{
			const size_t sz = static_cast<size_t>(index + 1);
			if (m_materials.size() <= sz)
				m_materials.resize(sz);
			m_materials[index] = material;
		}

		inline ResourceID GetModel() const
		{
			return m_model;
		}

		inline ResourceID GetMaterial(uint32 index) const
		{
			return m_materials.at(index);
		}

		inline const Vector<ResourceID>& GetMaterials() const
		{
			return m_materials;
		}

		inline const Vector<CompModelNode>& GetNodes() const
		{
			return m_nodes;
		}

		inline Vector<CompModelNode>& GetNodes()
		{
			return m_nodes;
		}

		inline AnimationController& GetAnimationController()
		{
			return m_animationController;
		}

		inline Model* GetModelPtr() const
		{
			return m_modelPtr;
		}

	private:
		LINA_REFLECTION_ACCESS(CompModel);

		Model*				  m_modelPtr = nullptr;
		AnimationController	  m_animationController;
		Vector<CompModelNode> m_nodes;
		ResourceID			  m_model = 0;
		Vector<ResourceID>	  m_materials;
	};

	LINA_COMPONENT_BEGIN(CompModel, "Graphics")
	LINA_FIELD(CompModel, m_model, "Model", FieldType::ResourceID, GetTypeID<Model>());
	LINA_FIELD_VEC(CompModel, m_materials, "Materials", FieldType::ResourceID, ResourceID, GetTypeID<Material>())
	LINA_FIELD_LOCK0(CompModel, m_materials)
	LINA_CLASS_END(CompModel)

} // namespace Lina
