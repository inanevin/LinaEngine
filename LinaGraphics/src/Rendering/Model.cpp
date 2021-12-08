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

#include "Rendering/Model.hpp"
#include "Rendering/VertexArray.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Rendering/ModelLoader.hpp"
#include <stdio.h>
#include <cereal/archives/binary.hpp>
#include <fstream>

namespace LinaEngine::Graphics
{

	std::map<int, Model> Model::s_loadedMeshes;

	Model::~Model()
	{
		for (uint32 i = 0; i < m_vertexArrays.size(); i++)
			delete m_vertexArrays[i];


		m_vertexArrays.clear();
		m_indexedModelArray.clear();
		m_materialSpecArray.clear();
		m_materialIndexArray.clear();
	}

	Model& Model::CreateModel(const std::string& filePath, MeshParameters meshParams, int id, const std::string& paramsPath)
	{
		// Internal meshes are created with non-negative ids, user loaded ones should have default id of -1.
		if (id == -1) id = Utility::GetUniqueID();

		Model& mesh = s_loadedMeshes[id];
		mesh.SetParameters(meshParams);
		ModelLoader::LoadModel(filePath, &mesh, meshParams);

		if (mesh.GetIndexedModels().size() == 0)
		{
			LINA_CORE_WARN("Indexed model array is empty! The model with the name: {0} could not be found or model scene does not contain any mesh! Returning plane quad...", filePath);
			UnloadModel(id);
			return GetPrimitive(Primitives::Plane);
		}

		// Build vertex array for each mesh.
		for (uint32 i = 0; i < mesh.GetIndexedModels().size(); i++)
		{
			VertexArray* vertexArray = new VertexArray();
			vertexArray->Construct(RenderEngine::GetRenderDevice(), mesh.GetIndexedModels()[i], BufferUsage::USAGE_DYNAMIC_DRAW);
			mesh.GetVertexArrays().push_back(vertexArray);
		}

		// Set id
		mesh.m_meshID = id;
		mesh.m_path = filePath;
		mesh.m_paramsPath = paramsPath;

		LINA_CORE_TRACE("Mesh created. {0}", filePath);
		return s_loadedMeshes[id];
	}

	Model& Model::GetModel(int id)
	{
		if (!ModelExists(id))
		{
			// Mesh not found.
			LINA_CORE_WARN("Mesh with the id {0} was not found, returning un-constructed mesh...", id);
			return Model();
		}

		return s_loadedMeshes[id];
	}

	Model& Model::GetModel(const std::string& path)
	{
		const auto it = std::find_if(s_loadedMeshes.begin(), s_loadedMeshes.end(), [path]
		(const auto& item) -> bool { return item.second.GetPath().compare(path) == 0; });

		if (it == s_loadedMeshes.end())
		{
			// Mesh not found.
			LINA_CORE_WARN("Mesh with the path {0} was not found, returning un-constructed mesh...", path);
			return Model();
		}

		return it->second;
	}

	bool Model::ModelExists(int id)
	{
		if (id < 0) return false;
		return !(s_loadedMeshes.find(id) == s_loadedMeshes.end());
	}

	bool Model::ModelExists(const std::string& path)
	{
		const auto it = std::find_if(s_loadedMeshes.begin(), s_loadedMeshes.end(), [path]
		(const auto& it) -> bool { 	return it.second.GetPath().compare(path) == 0; 	});
		return it != s_loadedMeshes.end();
	}

	void Model::UnloadModel(int id)
	{
		if (!ModelExists(id))
		{
			LINA_CORE_WARN("Mesh not found! Aborting... ");
			return;
		}

		s_loadedMeshes.erase(id);
	}

	Model& Model::GetPrimitive(Primitives primitive)
	{
		if (!ModelExists(primitive))
		{
			// VA not found.
			LINA_CORE_WARN("Primitive with the ID {0} was not found, returning plane...", primitive);
			return GetPrimitive(Primitives::Plane);
		}
		else
			return s_loadedMeshes[primitive];
	}

	void Model::UnloadAll()
	{
		s_loadedMeshes.clear();
	}

	MeshParameters Model::LoadParameters(const std::string& path)
	{
		MeshParameters params;

		std::ifstream stream(path);
		{
			cereal::BinaryInputArchive iarchive(stream);

			// Read the data into it.
			iarchive(params);
		}

		return params;
	}

	void Model::SaveParameters(const std::string& path, MeshParameters params)
	{
		std::ofstream stream(path);
		{
			cereal::BinaryOutputArchive oarchive(stream); // Build an output archive

			oarchive(params); // Write the data to the archive
		}
	}
}

