/*
Class: EngineSettings



Timestamp: 1/6/2022 6:58:17 PM
*/

#pragma once

#ifndef EngineSettings_HPP
#define EngineSettings_HPP

// Headers here.
#include "Data/Vector.hpp"
#include "Data/Serialization/VectorSerialization.hpp"
#include "Data/String.hpp"

namespace Lina
{
    class Engine;

    LINA_CLASS("Engine Settings")
    class EngineSettings : public Resources::IResource
    {

    public:
        EngineSettings()  = default;
        ~EngineSettings() = default;

        // Inherited via IResource
        virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const String& path) override;

        inline const Vector<String>& GetPackagedLevels()
        {
            return m_packagedLevels;
        }

    private:
        friend class Engine;
        friend class cereal::access;

        // These are the sid accessors for the levels cooked with the game packages.
        // Level manager uses the first one to load
        Vector<String> m_packagedLevels;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_dummy);
        }

        int m_dummy = 0;
    };
} // namespace Lina

#endif
