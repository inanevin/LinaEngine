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

#include "Graphics/Components/ModelNodeComponent.hpp"
#include "Serialization/HashMapSerialization.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Graphics/Resource/Model.hpp"
#include "Graphics/Resource/ModelNode.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/Shader.hpp"

namespace Lina
{
	void ModelNodeComponent::SaveToStream(OStream& stream)
	{
		RenderableComponent::SaveToStream(stream);
		stream << model << nodeIndex;
		HashMapSerialization::SaveToStream_PT(stream, materials);
	}

	void ModelNodeComponent::LoadFromStream(IStream& stream)
	{
		RenderableComponent::LoadFromStream(stream);
		stream >> model >> nodeIndex;
		HashMapSerialization::LoadFromStream_PT(stream, materials);
	}

	AABB& ModelNodeComponent::GetAABB(ResourceManager* rm)
	{
		return rm->GetResource<Model>(model)->GetNodes()[nodeIndex]->GetAABB();
	}

	Bitmask16 ModelNodeComponent::GetDrawPasses(ResourceManager* rm)
	{
		Bitmask16 mask;

		for (auto& m : materials)
		{
			auto* material = rm->GetResource<Material>(m.second);
			auto* shader   = rm->GetResource<Shader>(material->GetShaderHandle());
			mask.Set(shader->GetDrawPassMask().GetValue());
		}

		return mask;
	}

	Vector<MeshMaterialPair> ModelNodeComponent::GetMeshMaterialPairs(ResourceManager* rm)
	{
		Vector<MeshMaterialPair> pairs;

		auto*		node   = rm->GetResource<Model>(model)->GetNodes()[nodeIndex];
		const auto& meshes = node->GetMeshes();

		uint32 index = 0;
		for (auto m : meshes)
		{
			MeshMaterialPair p;
			p.mesh	   = m;
			p.material = rm->GetResource<Material>(materials[index]);
			pairs.push_back(p);
			index++;
		}

		return pairs;
	}
} // namespace Lina
