#include "Rendering/ImageAssetData.hpp"

namespace Lina::Graphics
{
    void* ImageAssetData::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        *this = Resources::LoadArchiveFromMemory<ImageAssetData>(path, data, dataSize);
        IResource::SetSID(path);
        return static_cast<void*>(this);
    }
    void* ImageAssetData::LoadFromFile(const String& path)
    {
        *this = Resources::LoadArchiveFromFile<ImageAssetData>(path);
        IResource::SetSID(path);
        return static_cast<void*>(this);
    }
} // namespace Lina::Graphics