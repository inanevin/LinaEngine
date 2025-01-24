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
#include "Core/Graphics/Data/Mesh.hpp"
#include "Core/Graphics/Data/ModelNode.hpp"
#include "Core/Graphics/Data/ModelMaterial.hpp"
#include "Core/Graphics/Data/ModelSkin.hpp"
#include "Core/Graphics/Data/ModelAnimation.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include <LinaGX/Common/Defines/DataBuffers.hpp>

namespace LinaGX
{
	struct ModelNode;
}

namespace Lina
{
	class MeshManager;

	class Model : public Resource
	{
	public:
		struct ModelTexture
		{
			String				  name	 = "";
			LinaGX::TextureBuffer buffer = {};
		};

		struct Metadata
		{
			void			   SaveToStream(OStream& stream) const;
			void			   LoadFromStream(IStream& stream);
			Vector<ResourceID> materials;
		};

		static constexpr uint32 VERSION = 0;

		Model(ResourceID id, const String& name) : Resource(id, GetTypeID<Model>(), name){};
		virtual ~Model();

		virtual bool   LoadFromFile(const String& path) override;
		virtual void   SaveToStream(OStream& stream) const override;
		virtual void   LoadFromStream(IStream& stream) override;
		virtual size_t GetSize() const override;
		virtual void   GenerateHW() override;
		virtual void   DestroyHW() override;
		void		   DestroyTextureDefs();
		void		   Upload(MeshManager* mm);
		void		   RemoveUpload(MeshManager* mm);

		inline const AABB& GetAABB() const
		{
			return m_totalAABB;
		}

		inline const Vector<ModelMaterial>& GetMaterialDefs() const
		{
			return m_materialDefs;
		}

		inline const Vector<ModelTexture>& GetTextureDefs() const
		{
			return m_textureDefs;
		}

		inline Metadata& GetMeta()
		{
			return m_meta;
		}

		inline const Metadata& GetMeta() const
		{
			return m_meta;
		}

		inline const Vector<ModelNode>& GetAllNodes() const
		{
			return m_allNodes;
		}

		inline const Vector<Mesh>& GetAllMeshes() const
		{
			return m_allMeshes;
		}

		inline const Vector<ModelSkin>& GetAllSkins() const
		{
			return m_allSkins;
		}

		inline const Vector<ModelAnimation>& GetAllAnimations() const
		{
			return m_allAnimations;
		}

	private:
		ALLOCATOR_BUCKET_MEM;
		Metadata			   m_meta;
		Vector<Mesh>		   m_allMeshes;
		Vector<ModelNode>	   m_allNodes;
		Vector<ModelMaterial>  m_materialDefs;
		Vector<ModelTexture>   m_textureDefs;
		Vector<ModelSkin>	   m_allSkins;
		Vector<ModelAnimation> m_allAnimations;
		AABB				   m_totalAABB;
	};
	LINA_RESOURCE_BEGIN(Model);
	LINA_CLASS_END(Model);

	LINA_CLASS_BEGIN(ModelMeta)
	LINA_FIELD_VEC(Model::Metadata, materials, "Materials", FieldType::ResourceID, ResourceID, GetTypeID<Material>());
	LINA_FIELD_LOCK0(Model::Metadata, materials)
	LINA_CLASS_END(ModelMeta)
} // namespace Lina
