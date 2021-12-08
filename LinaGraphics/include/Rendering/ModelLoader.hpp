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
Class: ModelLoader

Assimp wrapper, responsible for loading modals and creating the necessary Mesh data.

Timestamp: 4/26/2019 12:07:47 AM
*/

#pragma once

#ifndef ModelLoader_HPP
#define ModelLoader_HPP


#include "Animation/Animation.hpp"
#include "Material.hpp"
#include "Animation/Skeleton.hpp"
#include "Model.hpp"
#include <string>

namespace LinaEngine::Graphics
{

	class ModelLoader
	{
	public:

		// Load models using ASSIMP
		static bool LoadModel(const std::string& fileName, Model& model, ModelParameters meshParams);
		static bool LoadQuad(Mesh& model);
		static bool LoadPrimitive(std::vector<Mesh>& models, int vertexSize, int indicesSize, float* vertices, int* indices, float* texCoords);
		static void SetVertexBoneData(std::vector<int>& vertexBoneIDs, std::vector<float>& vertexBoneWeights, int boneID, float weight);
	};
}


#endif