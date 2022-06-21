/*
Class: AudioAssetData



Timestamp: 12/30/2021 10:04:49 PM
*/

#pragma once

#ifndef AudioAssetData_HPP
#define AudioAssetData_HPP

#include "Core/IResource.hpp"

namespace Lina::Audio
{
    class AudioAssetData : public Resources::IResource
    {

    public:
        AudioAssetData()          = default;
        virtual ~AudioAssetData() = default;

        virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const String& path) override;

        int m_dummy = 0;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_dummy);
        }
    };
} // namespace Lina::Audio

#endif
