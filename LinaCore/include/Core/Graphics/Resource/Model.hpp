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

#include "Core/Resources/Resource.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/Math/AABB.hpp"
#include "Core/Graphics/Data/ModelMaterial.hpp"

namespace LinaGX
{
	struct ModelNode;
}

namespace Lina
{
	class ModelNode;
	class MeshDefault;
	class MeshManager;

	class Model : public Resource
	{
	public:
		static constexpr uint32 VERSION = 0;

		Model(ResourceID id, const String& name) : Resource(id, GetTypeID<Model>(), name){};
		virtual ~Model();

		virtual void LoadFromFile(const String& path) override;
		virtual void SaveToStream(OStream& stream) const override;
		virtual void LoadFromStream(IStream& stream) override;
		void		 UploadNodes(MeshManager& meshManager);

		ModelNode* GetFirstNodeWMesh();

		inline const Vector<ModelNode*>& GetRootNodes() const
		{
			return m_rootNodes;
		}

		inline MeshDefault* GetMesh(uint32 index) const
		{
			return m_meshes.at(index);
		}

		inline const Vector<MeshDefault*>& GetMeshes() const
		{
			return m_meshes;
		}

		inline const AABB& GetAABB() const
		{
			return m_totalAABB;
		}

	private:
		void	   ProcessNode(LinaGX::ModelNode* lgxNode, ModelNode* parent);
		void	   UploadNode(MeshManager& meshManager, ModelNode* node);
		ModelNode* GetNodeWithMesh(ModelNode* root);

	private:
		ALLOCATOR_BUCKET_MEM;
		Vector<MeshDefault*>  m_meshes;
		Vector<ModelNode*>	  m_rootNodes;
		Vector<ModelMaterial> m_materials;
		AABB				  m_totalAABB;
	};
	LINA_RESOURCE_BEGIN(Model);
	LINA_CLASS_END(Model);
} // namespace Lina
