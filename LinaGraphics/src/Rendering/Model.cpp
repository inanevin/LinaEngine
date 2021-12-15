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
#include "Log/Log.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "Core/RenderEngineBackend.hpp"
#include <stdio.h>
#include <cereal/archives/binary.hpp>
#include <fstream>

namespace Lina::Graphics
{

	std::map<StringIDType, Model> Model::s_loadedModels;

	Model::~Model()
	{
		m_meshes.clear();
		m_materialSpecArray.clear();
	}

	Model& Model::CreateModel(StringIDType sid, const void* scene, ModelParameters meshParams, const std::string& filePath, const std::string& paramsPath)
	{
		Model& model = s_loadedModels[sid];
		model.SetParameters(meshParams);
		ModelLoader::LoadModel(scene, model);

		if (model.GetMeshes().size() == 0)
		{
			LINA_WARN("Indexed model array is empty! The model with the name: {0} could not be found or model scene does not contain any mesh! Returning plane quad...");
			UnloadModel(sid);
			return s_loadedModels[0];
		}

		// Build vertex array for each model.
		for (uint32 i = 0; i < model.GetMeshes().size(); i++)
		{
			model.GetMeshes()[i].CreateVertexArray( BufferUsage::USAGE_DYNAMIC_DRAW);
		}

		// Set id
		model.m_meshID = sid;
		model.m_path = filePath;
		model.m_paramsPath = paramsPath;
		return s_loadedModels[sid];
	}

	Model& Model::CreateModel(const std::string& filePath, ModelParameters meshParams,  const std::string& paramsPath)
	{
		StringIDType id = StringID(filePath.c_str()).value();

		Model& model = s_loadedModels[id];
		model.SetParameters(meshParams);
		ModelLoader::LoadModel(filePath, model, meshParams);

		if (model.GetMeshes().size() == 0)
		{
			LINA_WARN("Indexed model array is empty! The model with the name: {0} could not be found or model scene does not contain any mesh! Returning default mesh {0}", filePath);
			UnloadModel(id);
			return s_loadedModels[0];
		}

		// Build vertex array for each model.
		for (uint32 i = 0; i < model.GetMeshes().size(); i++)
		{
			model.GetMeshes()[i].CreateVertexArray( BufferUsage::USAGE_DYNAMIC_DRAW);
		}

		// Set id
		model.m_meshID = id;
		model.m_path = filePath;
		model.m_paramsPath = paramsPath;

		LINA_TRACE("[Mesh Loader] -> Mesh loaded from file: {0}", filePath);
		return s_loadedModels[id];
	}

	Model& Model::GetModel(StringIDType id)
	{
		if (!ModelExists(id))
		{
			// Mesh not found.
			LINA_WARN("Mesh with the id was not found, returning un-constructed mesh...");
			return Model();
		}

		return s_loadedModels[id];
	}

	Model& Model::GetModel(const std::string& path)
	{
		return GetModel(StringID(path.c_str()).value());
	}

	bool Model::ModelExists(StringIDType id)
	{
		if (id < 0) return false;
		return !(s_loadedModels.find(id) == s_loadedModels.end());
	}

	bool Model::ModelExists(const std::string& path)
	{
		return ModelExists(StringID(path.c_str()).value());
	}

	void Model::UnloadModel(StringIDType id)
	{
		if (!ModelExists(id))
		{
			LINA_WARN("Mesh not found! Aborting... ");
			return;
		}

		s_loadedModels.erase(id);
	}

	void Model::UnloadAll()
	{
		s_loadedModels.clear();
	}

	
}

