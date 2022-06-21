/*
Class: Audio



Timestamp: 12/15/2021 2:12:28 PM
*/

#pragma once

#ifndef Audio_HPP
#define Audio_HPP

// Headers here.
#include "Utility/StringId.hpp"
#include "Audio/AudioAssetData.hpp"
#include "Core/IResource.hpp"
#include "Data/Map.hpp"
#include <Data/String.hpp>

namespace Lina::Audio
{
    class Audio : public Resources::IResource
    {

    public:
        Audio()
        {
        }
        ~Audio();

        virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const String& path) override;

        unsigned int GetBuffer()
        {
            return m_buffer;
        }

    private:
        static void CheckForError();

    private:
        AudioAssetData* m_assetData = nullptr;
        int             m_size      = 0;
        int             m_format    = 0;
        unsigned int    m_buffer    = 0;
        float           m_freq      = 0.0f;
        void*           m_data      = nullptr;
    };
} // namespace Lina::Audio

#endif
