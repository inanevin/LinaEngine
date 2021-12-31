/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Rendering/Texture.hpp"
#include "Resources/ResourceStorage.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "Log/Log.hpp"
#include "Rendering/ArrayBitmap.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace Lina::Graphics
{
    Texture::~Texture()
    {
        m_id = m_renderDevice->ReleaseTexture2D(m_id);
    }

    void Texture::Construct(const ArrayBitmap& data, SamplerParameters samplerParams, bool shouldCompress, const std::string& path)
    {
        m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();
        m_size         = Vector2i((unsigned int)data.GetWidth(), (unsigned int)data.GetHeight());
        m_bindMode     = TextureBindMode::BINDTEXTURE_TEXTURE2D;
        m_sampler.Construct(samplerParams, m_bindMode);
        m_id = m_renderDevice->CreateTexture2D(m_size, data.GetPixelArray(), samplerParams, shouldCompress, false, Color::White);
        m_sampler.SetTargetTextureID(m_id);
        m_isCompressed = shouldCompress;
        m_hasMipMaps   = samplerParams.m_textureParams.m_generateMipMaps;
        m_isEmpty      = false;
        SetSID(path);
    }

    void Texture::ConstructCubemap(SamplerParameters samplerParams, const std::vector<ArrayBitmap*>& data, bool shouldCompress, const std::string& path)
    {
        if (data.size() != 6)
        {
            LINA_WARN("Could not construct cubemap texture! ArrayBitmap data size needs to be 6, returning un-constructed texture...");
            return;
        }

        m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();
        m_size         = Vector2i((unsigned int)data[0]->GetWidth(), (unsigned int)data[0]->GetHeight());
        m_bindMode     = TextureBindMode::BINDTEXTURE_CUBEMAP;

        std::vector<unsigned char*> cubeMapData;

        for (uint32 i = 0; i < 6; i++)
            cubeMapData.push_back(data[i]->GetPixelArray());

        m_sampler.Construct(samplerParams, m_bindMode);
        m_id = m_renderDevice->CreateCubemapTexture(m_size, samplerParams, cubeMapData, 6U);
        m_sampler.SetTargetTextureID(m_id);
        m_isCompressed = shouldCompress;
        m_hasMipMaps   = samplerParams.m_textureParams.m_generateMipMaps;
        m_isEmpty      = false;
        SetSID(path);
        cubeMapData.clear();
    }

    void Texture::ConstructHDRI(SamplerParameters samplerParams, Vector2i size, float* data, const std::string& path)
    {
        m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();
        m_size         = size;
        m_bindMode     = TextureBindMode::BINDTEXTURE_TEXTURE2D;
        m_sampler.Construct(samplerParams, m_bindMode);
        m_id = m_renderDevice->CreateTextureHDRI(size, data, samplerParams);
        m_sampler.SetTargetTextureID(m_id);
        m_isCompressed = false;
        m_hasMipMaps   = samplerParams.m_textureParams.m_generateMipMaps;
        m_isEmpty      = false;
        SetSID(path);
    }

    void Texture::ConstructRTCubemapTexture(Vector2i size, SamplerParameters samplerParams, const std::string& path)
    {
        m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();
        m_size         = size;
        m_bindMode     = TextureBindMode::BINDTEXTURE_CUBEMAP;
        m_sampler.Construct(samplerParams, m_bindMode, true);
        m_id = m_renderDevice->CreateCubemapTextureEmpty(m_size, samplerParams);
        m_sampler.SetTargetTextureID(m_id);
        m_isCompressed = false;
        m_isEmpty      = false;
        m_hasMipMaps   = samplerParams.m_textureParams.m_generateMipMaps;
        SetSID(path);
    }

    void Texture::ConstructRTTexture(Vector2i size, SamplerParameters samplerParams, bool useBorder, const std::string& path)
    {
        // Frame buffer texture.
        m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();
        SamplerParameters params;
        m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
        m_sampler.Construct(params, m_bindMode);
        m_id = m_renderDevice->CreateTexture2D(size, NULL, samplerParams, false, useBorder, Color::White);
        m_sampler.SetTargetTextureID(m_id);
        m_size         = size;
        m_isCompressed = false;
        m_isEmpty      = false;
        m_hasMipMaps   = false;
        SetSID(path);
    }

    void Texture::ConstructRTTextureMSAA(Vector2i size, SamplerParameters samplerParams, int sampleCount, const std::string& path)
    {
        m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();
        m_bindMode     = TextureBindMode::BINDTEXTURE_TEXTURE2D_MULTISAMPLE;
        m_id           = m_renderDevice->CreateTexture2DMSAA(size, samplerParams, sampleCount);
        m_size         = size;
        m_isCompressed = false;
        m_isEmpty      = false;
        m_hasMipMaps   = false;
        SetSID(path);
    }

    void Texture::ConstructEmpty(SamplerParameters samplerParams, const std::string& path)
    {
        m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();
        SamplerParameters params;
        m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
        m_sampler.Construct(params, m_bindMode);
        m_id = m_renderDevice->CreateTexture2DEmpty(Vector2::One, samplerParams);
        m_sampler.SetTargetTextureID(m_id);
        m_size         = Vector2::One;
        m_isCompressed = false;
        m_hasMipMaps   = false;
        m_isEmpty      = true;
        SetSID(path);
    }
    
    void* Texture::LoadFromMemory(const std::string& path, unsigned char* data, size_t dataSize)
    {
        LINA_TRACE("[Texture Loader - Memory] -> Loading: {0}", path);

        const std::string extension = Utility::GetFileExtension(Utility::GetFileNameOnly(path));
        if (extension.compare("hdr") == 0)
            return LoadFromMemoryHDRI(path, data, dataSize);

        IResource::SetSID(path);
        ArrayBitmap* textureBitmap = new ArrayBitmap();

        int nrComponents = textureBitmap->Load(data, dataSize);
        if (nrComponents == -1)
        {
            LINA_WARN("Texture with the path {0} doesn't exist, returning empty texture", path);
            delete textureBitmap;
            return static_cast<void*>(RenderEngineBackend::Get()->GetDefaultTexture());
        }

        const std::string  fileNameNoExt = Utility::GetFileWithoutExtension(path);
        const std::string  assetData     = fileNameNoExt + ".linaimagedata";
        const StringIDType assetDataSid  = StringID(assetData.c_str()).value();
        auto*              storage       = Resources::ResourceStorage::Get();

        if (storage->Exists<ImageAssetData>(assetDataSid))
        {
            m_assetData = storage->GetResource<ImageAssetData>(assetDataSid);
        }
        else
        {
            m_assetData = new ImageAssetData();
            storage->Add(static_cast<void*>(m_assetData), GetTypeID<ImageAssetData>(), assetDataSid);
        }

        Construct(*textureBitmap, m_assetData->m_samplerParameters, false, path);

        // Delete pixel data.
        delete textureBitmap;

        // Return
        return static_cast<void*>(this);
    }

    void* Texture::LoadFromFile(const std::string& path)
    {
        LINA_TRACE("[Texture Loader - File] -> Loading: {0}", path);

        const std::string extension = Utility::GetFileExtension(Utility::GetFileNameOnly(path));
        if (extension.compare("hdr") == 0)
            return LoadFromFileHDRI(path);

        IResource::SetSID(path);
        ArrayBitmap* textureBitmap = new ArrayBitmap();
        
        int nrComponents = textureBitmap->Load(path);
        if (nrComponents == -1)
        {
            LINA_WARN("Texture with the path {0} doesn't exist, returning empty texture", path);
            delete textureBitmap;
            return static_cast<void*>(RenderEngineBackend::Get()->GetDefaultTexture());
        }

        const std::string  fileNameNoExt = Utility::GetFileWithoutExtension(path);
        const std::string  assetData     = fileNameNoExt + ".linaimagedata";
        const StringIDType assetDataSid  = StringID(assetData.c_str()).value();
        auto*              storage       = Resources::ResourceStorage::Get();

        if (storage->Exists<ImageAssetData>(assetDataSid))
        {
            m_assetData = storage->GetResource<ImageAssetData>(assetDataSid);
        }
        else
        {
            m_assetData = new ImageAssetData();
            Resources::SaveArchiveToFile(assetData, *m_assetData);
            storage->Add(static_cast<void*>(m_assetData), GetTypeID<ImageAssetData>(), assetDataSid);
        }

        Construct(*textureBitmap, m_assetData->m_samplerParameters, false, path);

        // Delete pixel data.
        delete textureBitmap;

        // Return
        return static_cast<void*>(this);
    }

    void* Texture::LoadFromFileHDRI(const std::string& path)
    {
        // Build pixel data.
        SetSID(path);
        int    w, h, nrComponents;
        float* data = ArrayBitmap::LoadImmediateHDRI(path.c_str(), w, h, nrComponents);

        if (!data)
        {
            LINA_WARN("Texture with the path {0} doesn't exist, returning empty texture", path);
            return RenderEngineBackend::Get()->GetDefaultTexture();
        }

        // Build texture & construct.
        SamplerParameters samplerParams;
        samplerParams.m_textureParams.m_wrapR = samplerParams.m_textureParams.m_wrapS = samplerParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
        samplerParams.m_textureParams.m_minFilter = samplerParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
        samplerParams.m_textureParams.m_internalPixelFormat                                   = PixelFormat::FORMAT_RGB16F;
        samplerParams.m_textureParams.m_pixelFormat                                           = PixelFormat::FORMAT_RGB;
        ConstructHDRI(samplerParams, Vector2i(w, h), data, path);

        // Return
        return static_cast<void*>(this);
    }

    void* Texture::LoadFromMemoryHDRI(const std::string& path, unsigned char* data, size_t dataSize)
    {
        // Build pixel data.
        SetSID(path);
        int    w, h, nrComponents;
        float* bitmapData = ArrayBitmap::LoadImmediateHDRI(data, dataSize, w, h, nrComponents);
        LINA_ASSERT(bitmapData != nullptr, "HDR texture could not be loaded!");

        // Build texture & construct.
        SamplerParameters samplerParams;
        samplerParams.m_textureParams.m_wrapR = samplerParams.m_textureParams.m_wrapS = samplerParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
        samplerParams.m_textureParams.m_minFilter = samplerParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
        samplerParams.m_textureParams.m_internalPixelFormat                                   = PixelFormat::FORMAT_RGB16F;
        samplerParams.m_textureParams.m_pixelFormat                                           = PixelFormat::FORMAT_RGB;
        ConstructHDRI(samplerParams, Vector2i(w, h), bitmapData, path);

        // Return
        return static_cast<void*>(this);
    }

} // namespace Lina::Graphics
