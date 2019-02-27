/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Vertex
Timestamp: 1/7/2019 2:10:38 PM

*/

#include "LinaPch.hpp"
#include "Vertex.hpp"  

namespace LinaEngine
{

	/*void Vertex::CalcNormals(const unsigned int* pIndices, unsigned int IndexCount, Vertex* pVertices, unsigned int VertexCount)
	{
		// Accumulate each triangle normal into each of the triangle vertices
		for (unsigned int i = 0; i < IndexCount; i += 3) {
			unsigned int Index0 = pIndices[i];
			unsigned int Index1 = pIndices[i + 1];
			unsigned int Index2 = pIndices[i + 2];
			Vector3F v1 = pVertices[Index1].GetPosition() - pVertices[Index0].GetPosition();
			Vector3F v2 = pVertices[Index2].GetPosition() - pVertices[Index0].GetPosition();
			Vector3F Normal = v1.Cross(v2);
			Normal.Normalize();

			pVertices[Index0].GetNormal() += Normal;
			pVertices[Index1].GetNormal() += Normal;
			pVertices[Index2].GetNormal() += Normal;
		}

		// Normalize all the vertex normals
		for (unsigned int i = 0; i < VertexCount; i++) {
			pVertices[i].GetNormal().Normalize();
		}
	}*/
}

