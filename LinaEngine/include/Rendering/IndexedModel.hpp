/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: IndexedModel
Timestamp: 4/26/2019 12:11:36 AM

*/

#pragma once

#ifndef IndexedModel_HPP
#define IndexedModel_HPP

#include "Utility/Math/AABB.hpp"
#include "PackageManager/PAMRenderEngine.hpp"

namespace LinaEngine::Graphics
{
	class IndexedModel
	{
	public:

		IndexedModel() : instancedElementsStartIndex((uint32)-1) {}
		uint32 createVertexArray(RenderEngine<PAMRenderEngine>& device, int bufferUsage) const;

		void allocateElement(uint32 elementSize);
		void setInstancedElementStartIndex(uint32 elementIndex);

		void addElement1f(uint32 elementIndex, float e0);
		void addElement2f(uint32 elementIndex, float e0, float e1);
		void addElement3f(uint32 elementIndex, float e0, float e1, float e2);
		void addElement4f(uint32 elementIndex, float e0, float e1, float e2, float e3);

		void addIndices1i(uint32 i0);
		void addIndices2i(uint32 i0, uint32 i1);
		void addIndices3i(uint32 i0, uint32 i1, uint32 i2);
		void addIndices4i(uint32 i0, uint32 i1, uint32 i2, uint32 i3);

		FORCEINLINE AABB getAABBForElementArray(uint32 index) 
		{
			return AABB(&elements[index][0], elements[index].size() / elementSizes[index]);
		}

		uint32 getNumIndices() const;

	private:

		LinaArray<uint32> indices;
		LinaArray<uint32> elementSizes;
		LinaArray<LinaArray<float>> elements;
		uint32 instancedElementsStartIndex;

	};
}


#endif