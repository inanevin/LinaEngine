#include "Core/EngineSettings.hpp"

namespace Lina
{
    void* EngineSettings::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        *this = Resources::LoadArchiveFromMemory<EngineSettings>(path, data, dataSize);
        IResource::SetSID(path);
        return static_cast<void*>(this);
    }
    void* EngineSettings::LoadFromFile(const String& path)
    {
        *this = Resources::LoadArchiveFromFile<EngineSettings>(path);
        IResource::SetSID(path);
        return static_cast<void*>(this);
    }
}