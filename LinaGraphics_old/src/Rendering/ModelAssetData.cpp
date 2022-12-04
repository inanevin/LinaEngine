#include "Rendering/ModelAssetData.hpp"

namespace Lina::Graphics
{
    void* ModelAssetData::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        *this = Resources::LoadArchiveFromMemory<ModelAssetData>(path, data, dataSize);
        Resources::IResource::SetSID(path);
        return static_cast<void*>(this);
    }
    void* ModelAssetData::LoadFromFile(const char* path)
    {
        *this = Resources::LoadArchiveFromFile<ModelAssetData>(path);
        Resources::IResource::SetSID(path);
        return static_cast<void*>(this);
    }
} // namespace Lina::Graphics