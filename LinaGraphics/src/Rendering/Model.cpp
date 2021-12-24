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
#include <cereal/archives/portable_binary.hpp>
#include <fstream>
#include "Rendering/Model.hpp"

namespace Lina::Graphics
{

	std::map<StringIDType, Model> Model::s_loadedModels;

	Model::~Model()
	{
		m_meshes.clear();
		m_materialSpecArray.clear();
	}

	ModelAssetData Model::LoadAssetData(const std::string& path)
	{
		ModelAssetData params;
		std::ifstream stream(path, std::ios::binary);
		{
			cereal::PortableBinaryInputArchive iarchive(stream);
			iarchive(params);
		}
		return params;
	}

	void Model::SaveAssetData(const std::string& path, ModelAssetData modelAssetData)
	{
		std::ofstream stream(path, std::ios::binary);
		{
			cereal::PortableBinaryOutputArchive oarchive(stream);
			oarchive(modelAssetData); 
		}
	}

	ModelAssetData Model::LoadAssetDataFromMemory(unsigned char* data, size_t dataSize)
	{
		ModelAssetData params;
		{
			std::string data((char*)data, dataSize);
			std::istringstream stream(data, std::ios::binary);
			{
				cereal::PortableBinaryInputArchive iarchive(stream);
				iarchive(params);
			}
		}
		return params;
	}

	Model& Model::CreateModel(const std::string& path, const std::string& assetDataPath, unsigned char* data, size_t dataSize, ModelAssetData modelAssetData)
	{
		StringIDType id = StringID(path.c_str()).value();

		Model& model = s_loadedModels[id];
		model.SetAssetData(modelAssetData);
		model.m_assetDataPath = assetDataPath;
		model.m_path = path;
		ModelLoader::LoadModel(data, dataSize, model, modelAssetData);

		LINA_ASSERT(model.GetMeshes().size() != 0, "Model does not contain any meshes, Lina expects all imported files to contain at least a single mesh!");
	
		// Build vertex array for each model.
		for (uint32 i = 0; i < model.GetMeshes().size(); i++)
		{
			model.GetMeshes()[i].CreateVertexArray(BufferUsage::USAGE_DYNAMIC_DRAW);
		}

		// Set id
		model.m_id = id;
		model.m_path = path;
		model.m_assetDataPath = assetDataPath;
		return s_loadedModels[id];
	}


	Model& Model::CreateModel(const std::string& filePath, ModelAssetData modelAssetData,  const std::string& assetDataPath)
	{
		StringIDType id = StringID(filePath.c_str()).value();

		Model& model = s_loadedModels[id];
		model.SetAssetData(modelAssetData);
		model.m_assetDataPath = assetDataPath;
		model.m_path = filePath;
		ModelLoader::LoadModel(filePath, model, modelAssetData);

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
		model.m_id = id;
		model.m_path = filePath;
		model.m_assetDataPath = assetDataPath;
		return s_loadedModels[id];
	}

	Model& Model::GetModel(StringIDType id)
	{
		bool model = ModelExists(id);
		LINA_ASSERT(model, "Model does not exist!");
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

