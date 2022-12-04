/*
Class: Texture

Represents a texture creatd in the GPU backend, is responsible for delegating the creation of
all different types of textures, 2D, 3D, cubemap, HDRI etc.

Timestamp: 1/7/2019 1:55:47 PM
*/

#pragma once

#ifndef Texture_HPP
#define Texture_HPP

#include "Core/RenderEngineFwd.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/ImageAssetData.hpp"

namespace Lina::Graphics
{
    class ArrayBitmap;
    class DDSTexture;

    class Texture : public Resources::IResource
    {

    public:
        Texture() = default;
        ~Texture();

        virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const char* path) override;
        void*         LoadFromFileHDRI(const String& path);
        void*         LoadFromMemoryHDRI(const String& path, unsigned char* data, size_t dataSize);
        void          WriteToFile(const String& path);

        void Construct(SamplerParameters samplerParams, bool shouldCompress, const String& path = "");
        void ConstructCubemap(SamplerParameters samplerParams, const Vector<class ArrayBitmap*>& data, bool compress, const String& path = "");
        void ConstructHDRI(SamplerParameters samplerParams, const Vector2i& size, float* data, const String& path = "");
        void ConstructRTCubemapTexture(Vector2i size, SamplerParameters samplerParams, const String& path = "");
        void ConstructRTTexture(Vector2i size, SamplerParameters samplerParams, bool useBorder = false, const String& path = "");
        void ConstructRTTextureMSAA(Vector2i size, SamplerParameters samplerParams, int sampleCount, const String& path = "");
        void ConstructEmpty(SamplerParameters samplerParams = SamplerParameters(), const String& path = "");

        inline ImageAssetData* GetAssetData()
        {
            return m_assetData;
        }
        inline uint32 GetID() const
        {
            return m_id;
        };
        inline uint32 GetSamplerID() const
        {
            return m_sampler.GetID();
        }
        inline Sampler& GetSampler()
        {
            return m_sampler;
        }
        inline bool IsCompressed() const
        {
            return m_isCompressed;
        }
        inline bool HasMipmaps() const
        {
            return m_hasMipMaps;
        }
        inline Vector2i GetSize()
        {
            return m_size;
        }
        inline bool GetIsEmpty()
        {
            return m_isEmpty;
        }
        inline int GetNumComponents()
        {
            return m_numComponents;
        }


    private:
        friend RenderEngine;

        TextureBindMode m_bindMode;
        Sampler         m_sampler;
        ImageAssetData* m_assetData     = nullptr;
        RenderDevice*   m_renderDevice  = nullptr;
        uint32          m_id            = 0;
        Vector2i        m_size          = Vector2::One;
        bool            m_isCompressed  = false;
        bool            m_hasMipMaps    = true;
        bool            m_isEmpty       = true;
        int             m_numComponents = 0;
        ArrayBitmap*    m_bitmap        = nullptr;
    };
} // namespace Lina::Graphics

#endif
