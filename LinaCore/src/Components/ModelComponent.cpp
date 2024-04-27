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

#include "Core/Components/ModelComponent.hpp"
#include "Common/Serialization/VectorSerialization.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Data/ModelNode.hpp"

namespace Lina
{
	void ModelComponent::SaveToStream(OStream& stream) const
	{
		m_model.SaveToStream(stream);
		VectorSerialization::SaveToStream_OBJ(stream, m_materials);
	}

	void ModelComponent::LoadFromStream(IStream& stream)
	{
		m_model.LoadFromStream(stream);
		VectorSerialization::LoadFromStream_OBJ(stream, m_materials);
	}

	void ModelComponent::SetModel(StringID sid)
	{
		m_model.sid = sid;
	}

	void ModelComponent::SetMaterial(uint32 index, StringID sid)
	{
		if (m_materials.size() <= index)
			m_materials.resize(index + 1);

		m_materials[index].sid = sid;
	}

	void ModelComponent::FetchResources(ResourceManager* rm)
	{
		m_model.raw = rm->GetResource<Model>(m_model.sid);
		for (auto& mat : m_materials)
			mat.raw = rm->GetResource<Material>(mat.sid);

		m_materialToMeshMap.clear();

		for (auto* r : m_model.raw->GetRootNodes())
			FillMeshes(r);
	}

	void ModelComponent::FillMeshes(ModelNode* node)
	{
		// for(auto* m : node->GetMeshes())
		// {
		//     Material* mat = m_materials[m->GetMaterialIndex()].raw;
		//     m_materialToMeshMap.insert(linatl::make_pair(mat, m));
		// }

		// for(auto* c : node->GetChildren())
		//     FillMeshes(c);
	}

} // namespace Lina
