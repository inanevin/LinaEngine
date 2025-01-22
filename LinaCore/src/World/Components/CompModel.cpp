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

#include "Core/World/Components/CompModel.hpp"
#include "Core/Graphics/Data/ModelNode.hpp"
#include "Core/Resources/ResourceManager.hpp"

namespace Lina
{
	void CompModel::SetModel(Model* model)
	{
		m_modelPtr = model;
		m_model	   = model->GetID();
		m_nodes.clear();
		const Vector<ResourceID> modelMaterials = m_modelPtr->GetMeta().materials;
		const Vector<ModelNode>& nodes			= model->GetAllNodes();

		const size_t nodesSz = nodes.size();
		m_nodes.resize(nodesSz);

		for (size_t i = 0; i < nodesSz; i++)
		{
			const ModelNode& modelNode = nodes.at(i);
			CompModelNode&	 compNode  = m_nodes[i];
			compNode.transform.SetLocalMatrix(modelNode.localMatrix);
			compNode.inverseBindTransform = modelNode.inverseBindMatrix;
			compNode.parentIndex		  = modelNode.parentIndex;
		}
	}

	Matrix4 CompModel::CalculateGlobalMatrix(int32 nodeIndex)
	{
		const CompModelNode& node = m_nodes.at(nodeIndex);
		if (node.parentIndex != -1)
			return CalculateGlobalMatrix(node.parentIndex) * node.transform.GetLocalMatrix();
		return GetEntity()->GetTransform().GetMatrix() * node.transform.GetLocalMatrix();
	}

	void CompModel::StoreReferences()
	{
		m_modelPtr = m_resourceManager->GetResource<Model>(m_model);

		if (!m_modelPtr)
			return;

		SetModel(m_modelPtr);
	}
} // namespace Lina
