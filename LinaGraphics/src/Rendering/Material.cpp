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

#include "Rendering/Material.hpp"

#include "Core/RenderEngineBackend.hpp"
#include "Log/Log.hpp"
#include "Rendering/RenderConstants.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Texture.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Resources/ResourceStorage.hpp"

namespace Lina::Graphics
{
    std::set<Material*> Material::s_shadowMappedMaterials;
    std::set<Material*> Material::s_hdriMaterials;

    void Material::PostLoadMaterialData()
    {
        auto* storage = Resources::ResourceStorage::Get();

        for (std::map<std::string, MaterialSampler2D>::iterator it = m_sampler2Ds.begin(); it != m_sampler2Ds.end(); ++it)
        {
            if (storage->Exists<Texture>(it->second.m_texture.m_sid))
            {
                SetTexture(it->first, storage->GetResource<Texture>(it->second.m_texture.m_sid), it->second.m_bindMode);
            }
        }

        if (storage->Exists<Shader>(m_shaderHandle.m_sid))
        {
            m_shaderHandle.m_value = storage->GetResource<Shader>(m_shaderHandle.m_sid);
        }
        else
        {
            m_shaderHandle.m_value = OpenGLRenderEngine::Get()->GetDefaultLitShader();
            m_shaderHandle.m_sid   = m_shaderHandle.m_value->GetSID();
        }
    }

    void Material::SetTexture(const std::string& textureName, Texture* texture, TextureBindMode bindMode)
    {
        if (!(m_sampler2Ds.find(textureName) == m_sampler2Ds.end()))
        {
            m_sampler2Ds[textureName].m_texture.m_value = texture;
            m_sampler2Ds[textureName].m_texture.m_sid = texture->GetSID();
            m_sampler2Ds[textureName].m_bindMode     = bindMode;
            m_sampler2Ds[textureName].m_isActive     = texture == nullptr ? false : true;
        }
        else
        {
            LINA_ERR("This material doesn't support texture slot with the name {0}, returning...", textureName);
            return;
        }
    }
    void Material::RemoveTexture(const std::string& textureName)
    {
        if (!(m_sampler2Ds.find(textureName) == m_sampler2Ds.end()))
        {
            m_sampler2Ds[textureName].m_texture.m_sid = -1;
            m_sampler2Ds[textureName].m_texture.m_value = nullptr;
            m_sampler2Ds[textureName].m_isActive     = false;
        }
        else
        {
            LINA_ERR("This material doesn't support texture slot with the name {0}, returning...", textureName);
            return;
        }
    }
    Texture* Material::GetTexture(const std::string& name)
    {
        if (!(m_sampler2Ds.find(name) == m_sampler2Ds.end()))
            return m_sampler2Ds[name].m_texture.m_value;
        else
        {
            LINA_WARN("This material doesn't support texture slot with the name {0}, returning empty texture", name);
            return Graphics::RenderEngineBackend::Get()->GetDefaultTexture();
        }
    }

    void Material::SetSurfaceType(MaterialSurfaceType type)
    {
        m_surfaceType = type;
        SetInt(MAT_SURFACETYPE, static_cast<int>(type));
    }

    void Material::SetInt(const std::string& name, int value)
    {
        m_ints[name] = value;

        if (name == MAT_SURFACETYPE)
            m_surfaceType = static_cast<MaterialSurfaceType>(value);
    }

    Material* Material::CreateMaterial(Shader* shader, const std::string& savePath)
    {
        auto* storage = Resources::ResourceStorage::Get();

        if (storage->Exists<Material>(savePath))
        {
            LINA_WARN("Material already exists with the given path. {0}", savePath);
            return storage->GetResource<Material>(savePath);
        }

        Material* mat = new Material();
        mat->SetSID(savePath);
        mat->SetShader(shader);
        mat->SetMaterialContainers();
        Resources::SaveArchiveToFile<Material>(savePath, *mat);
        storage->Add(static_cast<void*>(mat), GetTypeID<Material>(), mat->GetSID());
        return mat;
    }

    void* Material::LoadFromFile(const std::string& path)
    {
        LINA_TRACE("[Material Loader - File] -> Loading: {0}", path);

        *this = Resources::LoadArchiveFromFile<Material>(path);
        IResource::SetSID(path);
        auto* storage = Resources::ResourceStorage::Get();

        if (storage->Exists<Shader>(m_shaderHandle.m_sid))
            SetShader(storage->GetResource<Shader>(m_shaderHandle.m_sid), true);
        else
            SetShader(OpenGLRenderEngine::Get()->GetDefaultLitShader(), true);

        SetMaterialContainers();
        UpdateMaterialData();

        for (auto& sampler : m_sampler2Ds)
        {
            if (storage->Exists<Texture>(sampler.second.m_texture.m_sid))
                SetTexture(sampler.first, storage->GetResource<Texture>(sampler.second.m_texture.m_sid), sampler.second.m_bindMode);
        }

        return static_cast<void*>(this);
    }

    void* Material::LoadFromMemory(const std::string& path, unsigned char* data, size_t dataSize)
    {
        LINA_TRACE("[Material Loader - File] -> Loading: {0}", path);

        *this         = Resources::LoadArchiveFromMemory<Material>(path, data, dataSize);
        IResource::SetSID(path);
        auto* storage = Resources::ResourceStorage::Get();

        if (storage->Exists<Shader>(m_shaderHandle.m_sid))
            SetShader(storage->GetResource<Shader>(m_shaderHandle.m_sid), true);
        else
            SetShader(OpenGLRenderEngine::Get()->GetDefaultLitShader(), true);

        SetMaterialContainers();
        UpdateMaterialData();

        for (auto& sampler : m_sampler2Ds)
        {
            if (storage->Exists<Texture>(sampler.second.m_texture.m_sid))
                SetTexture(sampler.first, storage->GetResource<Texture>(sampler.second.m_texture.m_sid), sampler.second.m_bindMode);
        }

        return static_cast<void*>(this);
    }

    void Material::UpdateMaterialData()
    {
        auto* storage = Resources::ResourceStorage::Get();

        if (storage->Exists<Shader>(m_shaderHandle.m_sid))
        {
            Shader*                  shader = storage->GetResource<Shader>(m_shaderHandle.m_sid);
            const ShaderUniformData& data   = shader->GetUniformData();

            for (const auto& e : data.m_colors)
            {
                if (m_colors.find(e.first) == m_colors.end())
                    m_colors[e.first] = e.second;
            }

            for (const auto& e : data.m_floats)
            {
                if (m_floats.find(e.first) == m_floats.end())
                    m_floats[e.first] = e.second;
            }

            for (const auto& e : data.m_bools)
            {
                if (m_bools.find(e.first) == m_bools.end())
                    m_bools[e.first] = e.second;
            }

            for (const auto& e : data.m_ints)
            {
                if (m_ints.find(e.first) == m_ints.end())
                    m_ints[e.first] = e.second;
            }

            for (const auto& e : data.m_vector2s)
            {
                if (m_vector2s.find(e.first) == m_vector2s.end())
                    m_vector2s[e.first] = e.second;
            }

            for (const auto& e : data.m_vector3s)
            {
                if (m_vector3s.find(e.first) == m_vector3s.end())
                    m_vector3s[e.first] = e.second;
            }

            for (const auto& e : data.m_vector4s)
            {
                if (m_vector4s.find(e.first) == m_vector4s.end())
                    m_vector4s[e.first] = e.second;
            }

            for (const auto& e : data.m_matrices)
            {
                if (m_matrices.find(e.first) == m_matrices.end())
                    m_matrices[e.first] = e.second;
            }

            for (const auto& e : data.m_sampler2Ds)
            {
                if (m_sampler2Ds.find(e.first) == m_sampler2Ds.end())
                    m_sampler2Ds[e.first] = {
                        e.second.m_unit,
                        e.second.m_bindMode,
                        false,
                        Resources::ResourceHandle<Texture>(),
                    };
            }
        }
    }

    void Material::SetShader(Shader* shader, bool onlySetID)
    {
        m_shaderHandle.m_sid   = shader->GetSID();
        m_shaderHandle.m_value = shader;

        if (onlySetID)
            return;

        // Clear all shader related material data.
        m_sampler2Ds.clear();
        m_colors.clear();
        m_floats.clear();
        m_ints.clear();
        m_vector3s.clear();
        m_vector2s.clear();
        m_matrices.clear();
        m_vector4s.clear();
        m_isShadowMapped = false;
        m_isPBR          = shader->GetPath().compare("Resources/Engine/Shaders/PBR/PBRLitStandard.glsl") == 0;
        m_usesHDRI       = false;

        ShaderUniformData data = shader->GetUniformData();
        m_colors               = data.m_colors;
        m_floats               = data.m_floats;
        m_bools                = data.m_bools;
        m_ints                 = data.m_ints;
        m_vector2s             = data.m_vector2s;
        m_vector3s             = data.m_vector3s;
        m_vector4s             = data.m_vector4s;
        m_matrices             = data.m_matrices;

        for (std::map<std::string, ShaderSamplerData>::iterator it = data.m_sampler2Ds.begin(); it != data.m_sampler2Ds.end(); ++it)
            m_sampler2Ds[it->first] = {
                it->second.m_unit,
                it->second.m_bindMode,
                false,
                Resources::ResourceHandle<Texture>(),
            };
    }

    void Material::SetMaterialContainers()
    {
        if (m_usesHDRI)
            s_hdriMaterials.emplace(this);

        if (m_isShadowMapped)
            s_shadowMappedMaterials.emplace(this);
    }

} // namespace Lina::Graphics
