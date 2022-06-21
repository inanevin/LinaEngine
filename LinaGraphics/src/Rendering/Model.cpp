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
