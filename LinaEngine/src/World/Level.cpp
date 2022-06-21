#include "World/Level.hpp"
#include "Core/Engine.hpp"
#include "ECS/Registry.hpp"
#include "EventSystem/LevelEvents.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <fstream>
#include <stdio.h>

namespace Lina::World
{
    void Level::Install()
    {
        ECS::Registry::s_ecs = &m_registry;
        LINA_TRACE("Level installed: {0}", m_path);
    }

    Level::Level(const Level& lvl)
    {
        m_ambientColor  = lvl.m_ambientColor;
        m_usedResources = lvl.m_usedResources;
    }

    void* Level::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        std::string        dataStr((char*)data, dataSize);
        std::istringstream stream(dataStr, std::ios::binary);
        {
            cereal::PortableBinaryInputArchive iarchive(stream);
            iarchive(*this);
            m_registry.DeserializeComponentsInRegistry(iarchive);
        }
        IResource::SetSID(path);
        return static_cast<void*>(this);
    }

    void* Level::LoadFromFile(const String& path)
    {
        std::ifstream stream(path.c_str(), std::ios::binary);
        {
            cereal::PortableBinaryInputArchive iarchive(stream);
            iarchive(*this);
            m_registry.DeserializeComponentsInRegistry(iarchive);
        }
        IResource::SetSID(path);
        return static_cast<void*>(this);
    }

    void Level::Uninstall()
    {
        m_registry.clear();
        LINA_TRACE("Level uninstalled: {0}", m_path);
    }

    void Level::SaveToFile(const String& path)
    {
        if (Utility::FileExists(path))
            Utility::DeleteFileInPath(path);

        std::ofstream stream(path.c_str(), std::ios::binary);
        {
            cereal::PortableBinaryOutputArchive oarchive(stream);
            oarchive(*this);
            m_registry.SerializeComponentsInRegistry(oarchive);
        }
    }

    void Level::AddResourceReference(TypeID tid, StringIDType sid)
    {
        m_usedResources[tid].insert(sid);
    }

} // namespace Lina::World
