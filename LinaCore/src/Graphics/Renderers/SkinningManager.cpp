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

#include "Core/Graphics/Renderers/SkinningManager.hpp"
#include "Core/World/Components/CompModel.hpp"

namespace Lina
{
	void SkinningManager::OnComponentAdded(Component* c)
	{
		if (c->GetTID() == GetTypeID<CompModel>())
			m_compModels.push_back(static_cast<CompModel*>(c));
	}

	void SkinningManager::OnComponentsRemoved(Component* comp)
	{
		if (comp->GetTID() == GetTypeID<CompModel>())
		{
			auto it = linatl::find_if(m_compModels.begin(), m_compModels.end(), [comp](CompModel* c) -> bool { return c == comp; });
			if (it != m_compModels.end())
				m_compModels.erase(it);
		}
	}

	void SkinningManager::CalculateSkinningMatrices(JobExecutor& executor)
	{
		// TODO: View check.
		Vector<CompModel*> visibleModels = m_compModels;

		m_cpuBoneContainers.resize(visibleModels.size());

		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(visibleModels.size()), 1, [&](int i) {
			CompModel*				 comp  = visibleModels.at(i);
			const Vector<ModelSkin>& skins = comp->GetSkins();

			if (skins.empty())
				return;

			BoneContainer& cont = m_cpuBoneContainers.at(i);
			cont.compModel		= comp;

			Vector<CompModelNode>& nodes = comp->GetNodes();

			for (const ModelSkin& skin : skins)
			{
				cont.skins.push_back({});
				SkinData&	  skinData			= cont.skins.back();
				const Matrix4 rootGlobal		= skin.rootJointIndex != -1 ? comp->CalculateGlobalMatrix(skin.rootJointIndex) : comp->GetEntity()->GetTransform().ToMatrix();
				const Matrix4 rootGlobalInverse = rootGlobal.Inverse();

				for (uint32 jointIndex : skin.jointIndices)
				{
					const Matrix4 global = comp->CalculateGlobalMatrix(jointIndex);
					const Matrix4 bone	 = rootGlobalInverse * global * nodes.at(jointIndex).inverseBindTransform;
					skinData.matrices.push_back(bone);
				}
			}
		});
		executor.RunAndWait(tf);

		uint32 index = 0;
		for (BoneContainer& cont : m_cpuBoneContainers)
		{
			for (SkinData& skin : cont.skins)
			{
				skin.startIndex = index;
				index += static_cast<uint32>(skin.matrices.size());
			}
		}
	}

	void SkinningManager::SyncRender()
	{
		m_gpuBoneContainers = m_cpuBoneContainers;
		m_cpuBoneContainers = {};
	}

	uint32 SkinningManager::GetBoneIndex(CompModel* comp, uint32 nodeIndex)
	{
		const Vector<ModelSkin>& skins = comp->GetSkins();
		// uint32					 skinIndex = 0;
		//
		// for (const ModelSkin& skin : skins)
		// {
		// 	bool found = false;
		// 	for (uint32 idx : skin.jointIndices)
		// 	{
		// 		if (idx == nodeIndex)
		// 		{
		// 			found = true;
		// 			break;
		// 		}
		// 	}
		//
		// 	if (found)
		// 		break;
		//
		// 	skinIndex++;
		// }

		auto it = linatl::find_if(m_cpuBoneContainers.begin(), m_cpuBoneContainers.end(), [comp](const BoneContainer& cont) -> bool { return cont.compModel == comp; });
		LINA_ASSERT(it != m_cpuBoneContainers.end(), "");
		LINA_ASSERT(it->skins.size() < 2, "");
		return it->skins.at(0).startIndex;
	}

} // namespace Lina
