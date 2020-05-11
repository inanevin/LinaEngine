/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Mesh
Timestamp: 4/26/2019 12:12:01 AM

*/

#include "LinaPch.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/VertexArray.hpp"

namespace LinaEngine::Graphics
{
	Mesh::~Mesh()
	{
		for (uint32 i = 0; i < m_VertexArrays.size(); i++)
			delete m_VertexArrays[i];

		m_VertexArrays.clear();
		m_IndexedModelArray.clear();
		m_MaterialSpecArray.clear();
		m_MaterialIndexArray.clear();
	}
}

