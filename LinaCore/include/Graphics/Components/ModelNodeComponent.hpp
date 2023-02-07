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

#ifndef ModelNodeComponent_HPP
#define ModelNodeComponent_HPP

// Headers here.
#include "Graphics/Components/RenderableComponent.hpp"
#include "Reflection/ClassReflection.hpp"
#include "Data/HashMap.hpp"

namespace Lina
{

	class ModelNodeComponent : public RenderableComponent
	{
	public:
		ModelNodeComponent(){};
		virtual ~ModelNodeComponent() = default;

		virtual void					 SaveToStream(OStream& stream) override;
		virtual void					 LoadFromStream(IStream& stream) override;
		virtual AABB&					 GetAABB(ResourceManager* rm) override;
		virtual Bitmask16				 GetDrawPasses(ResourceManager* rm) override;
		virtual Vector<MeshMaterialPair> GetMeshMaterialPairs(ResourceManager* rm) override;

		virtual RenderableType GetType() override
		{
			return RenderableType::RenderableStaticMesh;
		}

		inline uint32 GetNodeIndex()
		{
			return nodeIndex;
		}

		HashMap<uint32, StringID> materials; // Index - material pair
		StringID				  model		= 0;
		uint32					  nodeIndex = 0;
	};

	LINA_REFLECTCOMPONENT_BEGIN(ModelNodeComponent, "Model Node", "Graphics")
	LINA_REFLECTCOMPONENT_END(ModelNodeComponent);
} // namespace Lina

#endif
