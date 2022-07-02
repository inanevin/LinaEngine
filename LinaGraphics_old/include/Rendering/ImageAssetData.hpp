/*
Class: ImageAssetData



Timestamp: 12/31/2021 8:18:02 PM
*/

#pragma once

#ifndef ImageAssetData_HPP
#define ImageAssetData_HPP

// Headers here.
#include "Resources/IResource.hpp"
#include "Rendering/RenderingCommon.hpp"

namespace Lina::Graphics
{
    class ImageAssetData : public Resources::IResource
    {

    public:
        ImageAssetData()  = default;
        ~ImageAssetData() = default;

        virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const String& path) override;

        SamplerParameters m_samplerParameters;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_samplerParameters);
        }
    };
} // namespace Lina::Graphics

#endif
