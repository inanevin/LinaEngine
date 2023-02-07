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

#ifndef Model_HPP
#define Model_HPP

#include "Resources/Core/IResource.hpp"
#include "Graphics/Data/Vertex.hpp"
#include "Data/String.hpp"
#include "Data/Vector.hpp"

namespace Lina
{
	class ModelNode;
	class Entity;
	class EntityWorld;
	class ModelLoader;

	class Model : public IResource
	{
	public:
		Model() = default;
		virtual ~Model();

		Entity* AddToWorld(EntityWorld* w);

		inline ModelNode* GetRootNode()
		{
			return m_rootNode;
		}

		inline const Vector<ModelNode*>& GetNodes()
		{
			return m_nodes;
		}

	protected:
		// Inherited via IResource
		virtual void LoadFromFile(const char* path);
		virtual void SaveToStream(OStream& stream) override;
		virtual void LoadFromStream(IStream& stream) override;

	private:
		Entity* CreateEntityForNode(Entity* parent, EntityWorld* w, ModelNode* n);

	private:
		friend class ModelLoader;

		// Runtime
		int				   m_numMaterials = 0;
		int				   m_numMeshes	  = 0;
		int				   m_numAnims	  = 0;
		int				   m_numVertices  = 0;
		int				   m_numBones	  = 0;
		ModelNode*		   m_rootNode	  = nullptr;
		Vector<ModelNode*> m_nodes;
	};

} // namespace Lina

#endif
