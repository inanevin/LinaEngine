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

#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Data/ModelNode.hpp"
#include "Core/Graphics/Data/Mesh.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Common/System/System.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/VectorSerialization.hpp"

namespace Lina
{

	Model::Model(System* sys, const String& path, StringID sid) : Resource(sys, path, sid, GetTypeID<Model>())
	{
		m_gfxManager = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
	};
	Model::~Model()
	{
		for (auto* n : m_rootNodes)
		{
			if (n->m_mesh)
				m_gfxManager->GetMeshManager().RemoveMesh(n->m_mesh);

			delete n;
		}
		m_rootNodes.clear();
	}

	void Model::ProcessNode(LinaGX::ModelNode* lgxNode, ModelNode* parent)
	{
		ModelNode* node = new ModelNode();
		node->m_parent	= parent;

		if (parent == nullptr)
			m_rootNodes.push_back(node);
		else
			parent->m_children.push_back(node);

		if (lgxNode->localMatrix.empty())
			node->m_localMatrix = Matrix4::TransformMatrix(lgxNode->position, Quaternion(lgxNode->quatRot.x, lgxNode->quatRot.y, lgxNode->quatRot.z, lgxNode->quatRot.w), lgxNode->scale);
		else
			node->m_localMatrix = Matrix4(lgxNode->localMatrix.data());

		node->m_name = lgxNode->name;

		if (lgxNode->mesh != nullptr)
		{
			LinaGX::ModelMesh* lgxMesh = lgxNode->mesh;
			MeshDefault*	   m	   = new MeshDefault();
			m_meshes.push_back(m);
			node->m_mesh = m;
			m->m_name	 = lgxMesh->name;
			m->m_node	 = node;
			m->m_primitives.resize(lgxMesh->primitives.size());

			Vector3 min = Vector3::Zero;
			Vector3 max = Vector3::Zero;

			for (size_t i = 0; i < lgxMesh->primitives.size(); i++)
			{
				auto* lgxPrim  = lgxMesh->primitives[i];
				auto& meshPrim = m->m_primitives[i];

				min = min.Min(Vector3(lgxPrim->minPosition.x, lgxPrim->minPosition.y, lgxPrim->minPosition.z));
				max = max.Max(Vector3(lgxPrim->maxPosition.x, lgxPrim->maxPosition.y, lgxPrim->maxPosition.z));

				meshPrim.m_materialIndex = lgxPrim->material ? lgxPrim->material->index : -1;
				meshPrim.m_startVertex	 = static_cast<uint32>(m->m_vertices.size());
				meshPrim.m_startIndex	 = static_cast<uint32>(m->m_indices16.size());

				for (uint32 i = 0; i < lgxPrim->vertexCount; i++)
				{
					VertexDefault vtx = {};
					vtx.pos			  = lgxPrim->positions[i];
					vtx.normal		  = lgxPrim->normals.empty() ? Vector3::Zero : lgxPrim->normals[i];
					vtx.uv			  = lgxPrim->texCoords.empty() ? Vector2::Zero : lgxPrim->texCoords[i];
					m->m_vertices.push_back(vtx);
				}

				LINA_ASSERT(lgxPrim->indexType == LinaGX::IndexType::Uint16, "");
				uint16* indices		= reinterpret_cast<uint16*>(lgxPrim->indices.data());
				size_t	indicesSize = lgxPrim->indices.size() / 2;
				m->m_indices16.insert(m->m_indices16.end(), indices, indices + indicesSize);
			}

			m->m_localAABB		  = AABB(min, max);
			m_totalAABB.boundsMin = m_totalAABB.boundsMin.Min(min);
			m_totalAABB.boundsMax = m_totalAABB.boundsMax.Max(max);
			m_totalAABB.UpdateHalfExtents();
		}

		for (auto* lgxChild : lgxNode->children)
			ProcessNode(lgxChild, node);
	}

	void Model::LoadFromFile(const char* path)
	{
		const String	  ext		= FileSystem::GetFileExtension(path);
		LinaGX::ModelData modelData = {};

		bool success = false;
		if (ext.compare("glb") == 0)
			success = LinaGX::LoadGLTFBinary(path, modelData);
		else
			success = LinaGX::LoadGLTFASCII(path, modelData);

		m_materials.resize(modelData.allMaterials.size());

		size_t idx = 0;
		for (auto* lgxMat : modelData.allMaterials)
		{
			auto& mat  = m_materials[idx];
			mat.m_name = lgxMat->name;
			idx++;
		}

		for (auto* lgxNode : modelData.rootNodes)
			ProcessNode(lgxNode, nullptr);
	}

	void Model::LoadFromStream(IStream& stream)
	{
		VectorSerialization::LoadFromStream_OBJ(stream, m_materials);

		uint32 sz = 0;
		stream >> sz;

		m_rootNodes.resize(static_cast<size_t>(sz));

		for (size_t i = 0; i < static_cast<size_t>(sz); i++)
		{
			ModelNode* node = new ModelNode();
			node->LoadFromStream(stream);
			m_rootNodes[i] = node;

			if (node->m_mesh != nullptr)
				m_meshes.push_back(node->m_mesh);
		}
	}

	void Model::SaveToStream(OStream& stream) const
	{
		VectorSerialization::SaveToStream_OBJ(stream, m_materials);

		stream << static_cast<uint32>(m_rootNodes.size());

		for (auto* node : m_rootNodes)
			node->SaveToStream(stream);
	}

	void Model::BatchLoaded()
	{
		for (auto* n : m_rootNodes)
			UploadNode(n);
	}

	void Model::UploadNode(ModelNode* node)
	{
		for (auto* node : m_rootNodes)
		{
			MeshDefault* mesh = node->GetMesh();

			if (mesh)
				m_gfxManager->GetMeshManager().AddMesh(mesh);

			for (auto* c : node->m_children)
				UploadNode(c);
		}
	}

	ModelNode* Model::GetFirstNodeWMesh()
	{
		for (auto* n : m_rootNodes)
		{
			ModelNode* node = GetNodeWithMesh(n);
			if (node)
				return node;
		}

		return nullptr;
	}

	ModelNode* Model::GetNodeWithMesh(ModelNode* root)
	{
		if (root->GetMesh() != nullptr)
			return root;

		for (auto* c : root->m_children)
		{
			ModelNode* node = GetNodeWithMesh(c);
			if (node)
				return node;
		}

		return nullptr;
	}

} // namespace Lina
