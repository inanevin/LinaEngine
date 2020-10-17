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

/*
Class: Mesh

Responsible for storing vertex array data generated into IndexedModels. MeshRenderers point to a particular
mesh instance in the engine.

Timestamp: 5/6/2019 4:23:45 PM
*/

#pragma once

#ifndef MESH_HPP
#define MESH_HPP


#include "Rendering/Texture.hpp"
#include "Rendering/IndexedModel.hpp"
#include "Rendering/Material.hpp"

namespace LinaEngine::Graphics
{
	class VertexArray;

	class Mesh
	{

	public:

		Mesh() {};

		virtual ~Mesh();


		VertexArray* GetVertexArray(uint32 index)
		{

			if (index >= m_vertexArrays.size() || index < 0 || m_vertexArrays.size() == 0)
			{
				LINA_CORE_ERR("Index is bigger than the vertex array size or vertex array size is 0. Returning nullptr!");
				return nullptr;
			}

			return m_vertexArrays.at(index);
		}

		std::vector<VertexArray*>& GetVertexArrays()
		{
			return m_vertexArrays;
		}

		std::vector<IndexedModel>& GetIndexedModels()
		{
			return m_indexedModelArray;
		}

		std::vector<ModelMaterial>& GetMaterialSpecs()
		{
			return m_materialSpecArray;
		}

		std::vector<uint32>& GetMaterialIndices()
		{
			return m_materialIndexArray;
		}

		void SetParameters(MeshParameters params) { m_parameters = params; }
		MeshParameters& GetParameters() { return m_parameters; }
		const std::string& GetPath() const { return m_path; }

		int m_MeshID = -1;
		std::string m_path = "";

	private:
		
		MeshParameters m_parameters;
		std::vector<VertexArray*> m_vertexArrays;
		std::vector<IndexedModel> m_indexedModelArray;
		std::vector<ModelMaterial> m_materialSpecArray;
		std::vector<uint32> m_materialIndexArray;

	};
}

#endif