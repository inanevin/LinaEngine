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

Responsible for storing vertex array data generated into Meshs. MeshRenderers point to a particular
mesh instance in the engine.

Timestamp: 5/6/2019 4:23:45 PM
*/

#pragma once

#ifndef MODEL_HPP
#define MODEL_HPP

#include "ECS/ECS.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Material.hpp"
#include "Animation/Skeleton.hpp"
#include "Core/SizeDefinitions.hpp"
#include "Rendering/ModelNode.hpp"

namespace Lina::Graphics
{
	class VertexArray;
	class ModelLoader;

	class Model
	{

	public:

		Model() {};
		virtual ~Model() {};

		static ModelAssetData LoadAssetData(const std::string& path);
		static void SaveAssetData(const std::string& path, ModelAssetData params);
		static ModelAssetData LoadAssetDataFromMemory(unsigned char* data, size_t dataSize);
		static Model& CreateModel(const std::string& path, const std::string& assetDataPath, unsigned char* data, size_t dataSize, ModelAssetData modelAssetData = ModelAssetData());
		static Model& CreateModel(const std::string& filePath, ModelAssetData modelAssetData = ModelAssetData(), const std::string& paramsPath = "");
		static Model& GetModel(StringIDType id);
		static Model& GetModel(const std::string& path);
		static bool ModelExists(StringIDType id);
		static bool ModelExists(const std::string& path);
		static void UnloadModel(StringIDType id);
		static void UnloadAll();
		static std::map<StringIDType, Model>& GetLoadedModels() { return s_loadedModels; }
		void SetAssetData(ModelAssetData params) { m_assetData = params; }

		ModelNode& GetRoot() { return m_rootNode; }
		ModelAssetData& GetAssetData() { return m_assetData; }
		std::vector<ImportedModelMaterial>& GetImportedMaterials() { return m_importedMaterials; }
		const std::string& GetPath() const { return m_path; }
		const std::string& GetAssetDataPath() const { return m_assetDataPath; }
		StringIDType GetID() { return m_id; }

	private:

		static std::map<StringIDType, Model> s_loadedModels;

		friend class OpenGLRenderEngine;
		friend class ModelLoader;
		std::string m_path = "";
		std::string m_assetDataPath = "";

		StringIDType m_id = -1;
		ModelAssetData m_assetData;
		ModelNode m_rootNode;
		std::vector<ImportedModelMaterial> m_importedMaterials;
	};
}

#endif