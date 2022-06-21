#include "Audio/AudioAssetData.hpp"

namespace Lina::Audio
{
    void* AudioAssetData::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        *this = Resources::LoadArchiveFromMemory<AudioAssetData>(path, data, dataSize);
        IResource::SetSID(path);
        return static_cast<void*>(this);
    }

    void* AudioAssetData::LoadFromFile(const String& path)
    {
        *this = Resources::LoadArchiveFromFile<AudioAssetData>(path);
        IResource::SetSID(path);
        return static_cast<void*>(this);
    }

} // namespace Lina::Audio
