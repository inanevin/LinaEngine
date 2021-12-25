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
Class: Model


Timestamp: 5/6/2019 4:23:45 PM
*/

#pragma once

#ifndef MODEL_HPP
#define MODEL_HPP

#include "Rendering/ModelNode.hpp"
#include "Rendering/RenderingCommon.hpp"

namespace Lina::Graphics
{
    class VertexArray;
    class ModelLoader;

    class Model
    {

    public:
        Model(){};
        virtual ~Model(){};

        /// <summary>
        /// De-serializes the asset data at the given path & returns it.
        /// </summary>
        /// <returns>Loaded asset data.</returns>
        static ModelAssetData LoadAssetData(const std::string& path);

        /// <summary>
        /// De-serializes asset data from a memory buffer & returns it.
        /// </summary>
        /// <returns>Loaded asset data.</returns>
        static ModelAssetData LoadAssetDataFromMemory(unsigned char* data, size_t dataSize);

        /// <summary>
        /// Loads a model file given via a memory buffer.
        /// </summary>
        /// <returns>Constructed model.</returns>
        static Model& CreateModel(const std::string& path, const std::string& assetDataPath, unsigned char* data, size_t dataSize, ModelAssetData& modelAssetData);

        /// <summary>
        /// Loads a model file with the given path & constructs a model out of it.
        /// </summary>
        /// <returns>Constructed model.</returns>
        static Model& CreateModel(const std::string& filePath, ModelAssetData& modelAssetData, const std::string& paramsPath = "");

        static Model&                         GetModel(StringIDType id);
        static Model&                         GetModel(const std::string& path);
        static bool                           ModelExists(StringIDType id);
        static bool                           ModelExists(const std::string& path);
        static void                           UnloadModel(StringIDType id);
        static void                           UnloadAll();
        static std::map<StringIDType, Model>& GetLoadedModels()
        {
            return s_loadedModels;
        }

        inline ModelAssetData& GetAssetData()
        {
            return m_assetData;
        }
        inline std::vector<ImportedModelMaterial>& GetImportedMaterials()
        {
            return m_importedMaterials;
        }
        inline const std::string& GetPath() const
        {
            return m_path;
        }
        inline const std::string& GetAssetDataPath() const
        {
            return m_assetDataPath;
        }
        inline StringIDType GetID()
        {
            return m_id;
        }
        inline void SetAssetData(ModelAssetData& data)
        {
            m_assetData = data;
        }
        ModelNode& GetRootNode()
        {
            return m_rootNode;
        }

        /// <summary>
        /// Serializes the given asset data to the path.
        /// </summary>
        void SaveAssetData(const std::string& path);

    private:
        friend class OpenGLRenderEngine;
        friend class ModelLoader;
        friend class ModelNode;

        static std::map<StringIDType, Model> s_loadedModels;
        std::string                          m_path          = "";
        std::string                          m_assetDataPath = "";
        StringIDType                         m_id            = -1;
        ModelAssetData                       m_assetData;
        ModelNode                            m_rootNode;
        std::vector<ImportedModelMaterial>   m_importedMaterials;
    };
} // namespace Lina::Graphics

#endif
