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
#include "Core/Graphics/MeshManager.hpp"
#include "Common/System/System.hpp"
#include "Common/FileSystem/FileSystem.hpp"

namespace Lina
{

	void Model::Metadata::SaveToStream(OStream& stream) const
	{
		stream << materials;
	}

	void Model::Metadata::LoadFromStream(IStream& stream)
	{
		stream >> materials;
	}

	Model::~Model()
	{
		DestroyTextureDefs();

		for (auto* n : m_rootNodes)
		{
			delete n;
		}
		m_rootNodes.clear();
	}

	void Model::DestroyTextureDefs()
	{
		for (ModelTexture& txt : m_textureDefs)
		{
			if (txt.buffer.pixels == nullptr)
				continue;

			delete[] txt.buffer.pixels;
			txt.buffer.pixels = nullptr;
		}
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
			node->m_mesh			   = m;
			node->m_meshIndex		   = static_cast<uint32>(m_meshes.size());
			m_meshes.push_back(m);
			m->m_name = lgxMesh->name;
			m->m_node = node;
			m->m_primitives.resize(lgxMesh->primitives.size());

			Vector3 min = Vector3::Zero;
			Vector3 max = Vector3::Zero;

			for (size_t i = 0; i < lgxMesh->primitives.size(); i++)
			{
				auto* lgxPrim  = lgxMesh->primitives[i];
				auto& meshPrim = m->m_primitives[i];

				min = min.Min(Vector3(lgxPrim->minPosition.x, lgxPrim->minPosition.y, lgxPrim->minPosition.z));
				max = max.Max(Vector3(lgxPrim->maxPosition.x, lgxPrim->maxPosition.y, lgxPrim->maxPosition.z));

				meshPrim.m_materialIndex = lgxPrim->material ? lgxPrim->material->index : 0;
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

	bool Model::LoadFromFile(const String& path)
	{
		if (!FileSystem::FileOrPathExists(path))
			return false;

		const String	  ext		= FileSystem::GetFileExtension(path);
		LinaGX::ModelData modelData = {};

		bool success = false;
		if (ext.compare("glb") == 0)
			success = LinaGX::LoadGLTFBinary(path.c_str(), modelData);
		else
			success = LinaGX::LoadGLTFASCII(path.c_str(), modelData);

		if (!success)
		{
			LINA_ERR("Failed loading model! {0}", path);
			return false;
		}

		m_materialDefs.resize(modelData.allMaterials.size());
		m_meta.materials.resize(m_materialDefs.size());

		size_t idx = 0;
		for (auto* lgxMat : modelData.allMaterials)
		{
			auto& mat	 = m_materialDefs[idx];
			mat.name	 = lgxMat->name;
			mat.isOpaque = lgxMat->isOpaque;
			mat.albedo	 = Color(lgxMat->baseColor);

			for (auto [type, index] : lgxMat->textureIndices)
				mat.textureIndices[static_cast<uint8>(type)] = index;

			idx++;
		}

		for (auto* lgxTexture : modelData.allTextures)
		{
			ModelTexture txt = {};
			txt.name		 = lgxTexture->name;

			const size_t sz = static_cast<size_t>(lgxTexture->buffer.width * lgxTexture->buffer.height * lgxTexture->buffer.bytesPerPixel);
			txt.buffer		= {
					 .pixels		= new uint8[sz],
					 .width			= lgxTexture->buffer.width,
					 .height		= lgxTexture->buffer.height,
					 .bytesPerPixel = lgxTexture->buffer.bytesPerPixel,
			 };

			MEMCPY(txt.buffer.pixels, lgxTexture->buffer.pixels, sz);
			m_textureDefs.push_back(txt);
		}

		for (auto* lgxNode : modelData.rootNodes)
			ProcessNode(lgxNode, nullptr);

		return true;
	}

	void Model::LoadFromStream(IStream& stream)
	{
		Resource::LoadFromStream(stream);
		uint32 version = 0;
		stream >> version;
		stream >> m_id;
		stream >> m_materialDefs;
		stream >> m_meta;
		stream >> m_totalAABB;

		uint32 sz = 0;
		stream >> sz;

		m_meshes.clear();
		m_rootNodes.resize(static_cast<size_t>(sz));

		for (size_t i = 0; i < static_cast<size_t>(sz); i++)
		{
			ModelNode* node = new ModelNode();
			node->LoadFromStream(stream);
			node->m_owner  = this;
			m_rootNodes[i] = node;

			if (node->m_mesh != nullptr)
				m_meshes.push_back(node->m_mesh);
		}
	}

	void Model::SaveToStream(OStream& stream) const
	{
		Resource::SaveToStream(stream);
		stream << VERSION;
		stream << m_id;
		stream << m_materialDefs;
		stream << m_meta;
		stream << m_totalAABB;

		stream << static_cast<uint32>(m_rootNodes.size());
		for (auto* node : m_rootNodes)
			node->SaveToStream(stream);
	}

	void Model::UploadNodes(MeshManager& meshManager)
	{
		for (ModelNode* node : m_rootNodes)
			UploadNode(meshManager, node);
	}

	void Model::UploadNode(MeshManager& meshManager, ModelNode* node)
	{
		MeshDefault* mesh = node->GetMesh();
		if (mesh)
			meshManager.AddMesh(mesh);

		for (auto* c : node->m_children)
			UploadNode(meshManager, c);
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
