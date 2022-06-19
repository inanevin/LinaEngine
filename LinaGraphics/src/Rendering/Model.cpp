/*
/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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
#include "Core/RenderEngine.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "Log/Log.hpp"
#include "Rendering/VertexArray.hpp"
#include "Utility/ModelLoader.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Resources/ResourceStorage.hpp"

namespace Lina::Graphics
{
    Model::~Model()
    {
        delete m_rootNode;
    }

    void* Model::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        LINA_TRACE("[Model Loader - Memory] -> Loading: {0}", path);

        SetSID(path);

        const String fileNameNoExt = Utility::GetFileWithoutExtension(path);
        const String assetDataPath = fileNameNoExt + ".linamodeldata";
        GetCreateAssetdata<ModelAssetData>(assetDataPath, m_assetData);

        ModelLoader::LoadModel(data, dataSize, this);

        // Set id
        return static_cast<void*>(this);
    }

    void* Model::LoadFromFile(const String& path)
    {
        LINA_TRACE("[Model Loader - File] -> Loading: {0}", path);

        SetSID(path);

        const String fileNameNoExt = Utility::GetFileWithoutExtension(path);
        const String assetDataPath = fileNameNoExt + ".linamodeldata";
        GetCreateAssetdata<ModelAssetData>(assetDataPath, m_assetData);

        ModelLoader::LoadModel(path, this);
        return static_cast<void*>(this);
    }

} // namespace Lina::Graphics
