#include "Rendering/ShaderInclude.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Log/Log.hpp"

namespace Lina::Graphics
{
    void* ShaderInclude::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        LINA_TRACE("[Shader Include Loader - Memory] -> Loading: {0}", path);
        const String& name = Utility::GetFileNameOnly(Utility::GetFileWithoutExtension(path));
        m_text = String(reinterpret_cast<char*>(data), dataSize);
        IResource::SetSID(path);
        return static_cast<void*>(this);
    }
    void* ShaderInclude::LoadFromFile(const String& path)
    {
        LINA_TRACE("[Shader Include Loader - File] -> Loading: {0}", path);
        const String& name = Utility::GetFileNameOnly(Utility::GetFileWithoutExtension(path));
        m_text = Utility::GetFileContents(path);
        IResource::SetSID(path);
        return static_cast<void*>(this);
    }
} // namespace Lina::Graphics