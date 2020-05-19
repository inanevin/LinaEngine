/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: ModelLoader
Timestamp: 4/26/2019 12:07:47 AM

*/

#pragma once

#ifndef ModelLoader_HPP
#define ModelLoader_HPP


#include "IndexedModel.hpp"
#include "Material.hpp"
#include <string>

namespace LinaEngine::Graphics
{
	class ModelLoader
	{
	public:

		// Load models using ASSIMP
		bool LoadModels(const std::string& fileName, LinaArray<IndexedModel>& models, LinaArray<uint32>& modelMaterialIndices, LinaArray<ModelMaterial>& materials);

		bool LoadPrimitive(LinaArray<IndexedModel>& models, int vertexSize, int indicesSize, float* vertices, int* indices, float* texCoords);
	};
}


#endif