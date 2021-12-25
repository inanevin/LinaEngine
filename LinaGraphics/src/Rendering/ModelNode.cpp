/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Rendering/ModelNode.hpp"
#include "Utility/AssimpUtility.hpp"
#include "Rendering/SkinnedMesh.hpp"
#include "Rendering/StaticMesh.hpp"
#include "Rendering/Model.hpp"
#include "Log/Log.hpp"
#include "Utility/ModelLoader.hpp"
#include <vector>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

namespace Lina::Graphics
{
	ModelNode::ModelNode()
	{
		LINA_TRACE("Constructed node {0}", m_name);
	}

	ModelNode::~ModelNode()
	{
		LINA_TRACE("Deleting Node {0}", m_name);
		for (uint32 i = 0; i < m_meshes.size(); i++)
			delete m_meshes[i];

		for (uint32 i = 0; i < m_children.size(); i++)
			delete m_children[i];

		m_children.clear();
		m_meshes.clear();
	}

	ModelNode::ModelNode(const ModelNode& old_obj)
	{
		LINA_TRACE("Copy constructor, old name {0}", old_obj.m_name);
	}

	void ModelNode::FillNodeHierarchy(const aiNode* node, const aiScene* scene, Model& parentModel, bool fillMeshesOnly)
	{
		if (!fillMeshesOnly)
		{
			m_name = std::string(node->mName.C_Str());
			const std::string sidName = parentModel.GetPath() + m_name;
			m_id = StringID(sidName.c_str()).value();
			m_localTransform = AssimpToLinaMatrix(node->mTransformation);
		}
		
		LINA_TRACE("Filling Node {0}", m_name);

		for (uint32 i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
			Mesh* addedMesh = nullptr;

			if (aimesh->HasBones())
			{
				SkinnedMesh* skinned = new SkinnedMesh();
				m_meshes.push_back(skinned);
				addedMesh = skinned;
			}
			else
			{
				StaticMesh* staticMesh = new StaticMesh();
				m_meshes.push_back(staticMesh);
				addedMesh = staticMesh;
			}

			ModelLoader::FillMeshData(aimesh, addedMesh);

			// Finally, construct the vertex array object for the mesh.
			addedMesh->CreateVertexArray(BufferUsage::USAGE_DYNAMIC_DRAW);
		}

		// Recursively fill the other nodes.
		for (uint32 i = 0; i < node->mNumChildren; i++)
		{
			if (!fillMeshesOnly)
			{
				LINA_TRACE("Adding Children {0}", node->mChildren[i]->mName.C_Str());
				ModelNode* newNode = new ModelNode();
				m_children.push_back(newNode);
				newNode->FillNodeHierarchy(node->mChildren[i], scene, parentModel, fillMeshesOnly);
			}
			else
				m_children[i]->FillNodeHierarchy(node->mChildren[i], scene, parentModel, fillMeshesOnly);
		
		}
		
	}
}