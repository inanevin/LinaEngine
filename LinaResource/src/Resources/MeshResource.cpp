/*
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

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

#include "Resources/MeshResource.hpp"
#include "EventSystem/Events.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cereal/archives/portable_binary.hpp>
#include <fstream>

namespace Lina::Resources
{

	bool MeshResource::LoadFromMemory(StringIDType sid, unsigned char* buffer, size_t bufferSize, ModelParameters& params)
	{
		// Get the importer & set assimp scene.
		Assimp::Importer importer;
		uint32_t importFlags = 0;

		if (params.m_calculateTangentSpace)
			importFlags |= aiProcess_CalcTangentSpace;

		if (params.m_triangulate)
			importFlags |= aiProcess_Triangulate;

		if (params.m_smoothNormals)
			importFlags |= aiProcess_GenSmoothNormals;

		if (params.m_flipUVs)
			importFlags |= aiProcess_FlipUVs;

		if (params.m_flipWinding)
			importFlags |= aiProcess_FlipWindingOrder;

		const aiScene* scene = importer.ReadFileFromMemory(buffer, bufferSize, importFlags);

		if (!scene)
		{
			LINA_ERR("[Mesh Loader] -> Mesh loading from memory failed!");
			return false;
		}

		// Trigger event w/ data
		Lina::Event::EventSystem::Get()->Trigger<Event::EModelResourceLoaded>({ sid, "", "", (void*)scene, params});

		LINA_TRACE("[Mesh Loader] -> Mesh loaded from memory.");
		return true;
	}

	bool MeshResource::LoadFromFile(const std::string& path, const std::string& paramsPath, ModelParameters& params)
	{
		// Get the importer & set assimp scene.
		Assimp::Importer importer;
		uint32_t importFlags = 0;

		if (params.m_calculateTangentSpace)
			importFlags |= aiProcess_CalcTangentSpace;

		if (params.m_triangulate)
			importFlags |= aiProcess_Triangulate;

		if (params.m_smoothNormals)
			importFlags |= aiProcess_GenSmoothNormals;

		if (params.m_flipUVs)
			importFlags |= aiProcess_FlipUVs;

		if (params.m_flipWinding)
			importFlags |= aiProcess_FlipWindingOrder;

		const aiScene* scene = importer.ReadFile(path.c_str(), importFlags);

		if (!scene)
		{
			LINA_ERR("[Mesh Loader] -> Mesh loading failed: {0}", path.c_str());
			return false;
		}
		
		// Trigger event w/ data
		Lina::Event::EventSystem::Get()->Trigger<Event::EModelResourceLoaded>({ StringID(path.c_str()).value(), path, paramsPath, (void*)scene, params });

		LINA_TRACE("[Mesh Loader] -> Mesh loaded from file: {0}", path);
		return true;
	}

	bool Lina::Resources::MeshResource::LoadParamsFromFile(const std::string& path, ModelParameters& params)
	{
		std::ifstream stream(path, std::ios::binary);
		{
			cereal::PortableBinaryInputArchive iarchive(stream);
			iarchive(params);
			LINA_TRACE("[Mesh Loader] -> Mesh params loaded from file: {0}", path);
		}

		return true;
	}

	bool Lina::Resources::MeshResource::SaveParamsToFile(const std::string& path, ModelParameters& params)
	{
		// Delete if exists.
		if (Utility::FileExists(path))
			Utility::DeleteFileInPath(path);

		std::ofstream levelDataStream(path, std::ios::binary);
		{
			cereal::PortableBinaryOutputArchive oarchive(levelDataStream);
			oarchive(params); 
		}

		return true;
	}

}