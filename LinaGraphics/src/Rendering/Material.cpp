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

#include <cereal/archives/portable_binary.hpp>
#include <fstream>
#include <stdio.h>

namespace Lina::Graphics
{

    std::map<StringIDType, Material> Material::s_loadedMaterials;
    std::set<Material*>              Material::s_shadowMappedMaterials;
    std::set<Material*>              Material::s_hdriMaterials;

    void Material::PostLoadMaterialData()
    {
        for (std::map<std::string, MaterialSampler2D>::iterator it = m_sampler2Ds.begin(); it != m_sampler2Ds.end(); ++it)
        {
            if (Texture::TextureExists(it->second.m_path))
            {
                SetTexture(it->first, &Texture::GetTexture(it->second.m_path), it->second.m_bindMode);
            }
        }

        if (Shader::ShaderExists(m_shaderPath))
        {
            m_shaderID  = Shader::GetShader(m_shaderPath).GetID();
            m_shaderSID = Shader::GetShader(m_shaderPath).GetSID();
        }
        else
        {
            m_shaderID  = OpenGLRenderEngine::GetDefaultShader().GetID();
            m_shaderSID = OpenGLRenderEngine::GetDefaultShader().GetSID();
        }
    }

    void Material::LoadMaterialData(Material& mat, const std::string& path)
    {
        std::ifstream stream(path, std::ios::binary);
        {
            cereal::PortableBinaryInputArchive iarchive(stream);
            iarchive(mat);
        }
    }

    void Material::SaveMaterialData(const Material& mat, const std::string& path)
    {
        std::ofstream stream(path, std::ios::binary);
        {
            cereal::PortableBinaryOutputArchive oarchive(stream);
            oarchive(mat);
        }
    }

    void Material::SetTexture(const std::string& textureName, Texture* texture, TextureBindMode bindMode)
    {
        if (!(m_sampler2Ds.find(textureName) == m_sampler2Ds.end()))
        {
            m_sampler2Ds[textureName].m_boundTexture = texture;
            m_sampler2Ds[textureName].m_bindMode     = bindMode;
            m_sampler2Ds[textureName].m_isActive     = texture == nullptr ? false : true;

            if (texture != nullptr)
            {
                m_sampler2Ds[textureName].m_path          = texture->GetPath();
                m_sampler2Ds[textureName].m_assetDataPath = texture->GetAssetDataPath();
            }
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
            m_sampler2Ds[textureName].m_boundTexture = nullptr;
            m_sampler2Ds[textureName].m_isActive     = false;
        }
        else
        {
            LINA_ERR("This material doesn't support texture slot with the name {0}, returning...", textureName);
            return;
        }
    }
    Texture& Material::GetTexture(const std::string& name)
    {
        if (!(m_sampler2Ds.find(name) == m_sampler2Ds.end()))
            return *m_sampler2Ds[name].m_boundTexture;
        else
        {
            LINA_WARN("This material doesn't support texture slot with the name {0}, returning empty texture", name);
            return Graphics::RenderEngineBackend::Get()->GetDefaultTexture();
        }
    }

    void Material::SetSurfaceType(MaterialSurfaceType type)
    {
        m_surfaceType = type;
        SetInt(MAT_SURFACETYPE, type);
    }

    void Material::SetInt(const std::string& name, int value)
    {
        m_ints[name] = value;

        if (name == MAT_SURFACETYPE)
            m_surfaceType = static_cast<MaterialSurfaceType>(value);
    }

    Material& Material::CreateMaterial(Shader& shader, const std::string& path)
    {
        StringIDType sid = StringID(path.c_str()).value();

        if (MaterialExists(sid))
        {
            LINA_WARN("Material already exists with the given path. {0}", path);
            return s_loadedMaterials[sid];
        }

        // Build material & set it's shader.
        Material& mat = s_loadedMaterials[sid];
        SetMaterialShader(mat, shader);
        SetMaterialContainers(mat);
        mat.m_materialID = sid;
        mat.m_path       = path.compare("") == 0 ? INTERNAL_MAT_PATH : path;
        return s_loadedMaterials[sid];
    }

    Material& Material::LoadMaterialFromFile(const std::string& path)
    {
        // Build material & set it's shader.
        StringIDType id  = StringID(path.c_str()).value();
        Material&    mat = s_loadedMaterials[id];
        Material::LoadMaterialData(mat, path);

        if (Shader::ShaderExists(mat.m_shaderPath))
            SetMaterialShader(mat, Shader::GetShader(mat.m_shaderPath), true);
        else
            SetMaterialShader(mat, OpenGLRenderEngine::GetDefaultShader(), true);

        SetMaterialContainers(mat);

        mat.m_materialID = id;
        mat.m_path       = path;
        mat.UpdateMaterialData();

        for (auto& sampler : mat.m_sampler2Ds)
        {
            if (Texture::TextureExists(sampler.second.m_path))
                mat.SetTexture(sampler.first, &Texture::GetTexture(sampler.second.m_path), sampler.second.m_bindMode);
        }

        return s_loadedMaterials[id];
    }

    Material& Material::LoadMaterialFromMemory(const std::string& path, unsigned char* data, size_t dataSize)
    {
        // Build material & set it's shader.
        StringIDType id  = StringID(path.c_str()).value();
        Material&    mat = s_loadedMaterials[id];

        {
            std::string        data((char*)data, dataSize);
            std::istringstream stream(data, std::ios::binary);
            {
                cereal::PortableBinaryInputArchive iarchive(stream);
                iarchive(mat);
            }
        }

        if (Shader::ShaderExists(mat.m_shaderPath))
            SetMaterialShader(mat, Shader::GetShader(mat.m_shaderPath), true);
        else
            SetMaterialShader(mat, OpenGLRenderEngine::GetDefaultShader(), true);

        SetMaterialContainers(mat);

        mat.m_materialID = id;
        mat.m_path       = path;
        mat.UpdateMaterialData();

        for (auto& sampler : mat.m_sampler2Ds)
        {
            if (Texture::TextureExists(sampler.second.m_path))
                mat.SetTexture(sampler.first, &Texture::GetTexture(sampler.second.m_path), sampler.second.m_bindMode);
        }

        return s_loadedMaterials[id];
    }

    void Material::UpdateMaterialData()
    {
        if (Shader::ShaderExists(m_shaderSID))
        {
            ShaderUniformData data = Shader::GetShader(m_shaderSID).GetUniformData();

            for (auto& e : data.m_colors)
            {
                if (m_colors.find(e.first) == m_colors.end())
                    m_colors[e.first] = e.second;
            }

            for (auto& e : data.m_floats)
            {
                if (m_floats.find(e.first) == m_floats.end())
                    m_floats[e.first] = e.second;
            }

            for (auto& e : data.m_bools)
            {
                if (m_bools.find(e.first) == m_bools.end())
                    m_bools[e.first] = e.second;
            }

            for (auto& e : data.m_ints)
            {
                if (m_ints.find(e.first) == m_ints.end())
                    m_ints[e.first] = e.second;
            }

            for (auto& e : data.m_vector2s)
            {
                if (m_vector2s.find(e.first) == m_vector2s.end())
                    m_vector2s[e.first] = e.second;
            }

            for (auto& e : data.m_vector3s)
            {
                if (m_vector3s.find(e.first) == m_vector3s.end())
                    m_vector3s[e.first] = e.second;
            }

            for (auto& e : data.m_vector4s)
            {
                if (m_vector4s.find(e.first) == m_vector4s.end())
                    m_vector4s[e.first] = e.second;
            }

            for (auto& e : data.m_matrices)
            {
                if (m_matrices.find(e.first) == m_matrices.end())
                    m_matrices[e.first] = e.second;
            }

            for (auto& e : data.m_sampler2Ds)
            {
                if (m_sampler2Ds.find(e.first) == m_sampler2Ds.end())
                    m_sampler2Ds[e.first] = {e.second.m_unit, nullptr, "", "", e.second.m_bindMode, false};
            }
        }
    }

    Material& Material::GetMaterial(StringIDType id)
    {
        bool materialExists = MaterialExists(id);
        LINA_ASSERT(materialExists, "Material does not exist!");
        return s_loadedMaterials[id];
    }

    Material& Material::GetMaterial(const std::string& path)
    {
        return GetMaterial(StringID(path.c_str()).value());
    }

    Material& Material::SetMaterialShader(Material& material, Shader& shader, bool onlySetID)
    {

        material.m_shaderID   = shader.GetID();
        material.m_shaderSID  = shader.GetSID();
        material.m_shaderPath = shader.GetPath();

        if (onlySetID)
            return material;

        // Clear all shader related material data.
        material.m_sampler2Ds.clear();
        material.m_colors.clear();
        material.m_floats.clear();
        material.m_ints.clear();
        material.m_vector3s.clear();
        material.m_vector2s.clear();
        material.m_matrices.clear();
        material.m_vector4s.clear();
        material.m_isShadowMapped = false;
        material.m_isPBR          = shader.GetPath().compare("Resources/Engine/Shaders/PBR/PBRLitStandard.glsl") == 0;
        material.m_usesHDRI       = false;

        ShaderUniformData data = shader.GetUniformData();
        material.m_colors      = data.m_colors;
        material.m_floats      = data.m_floats;
        material.m_bools       = data.m_bools;
        material.m_ints        = data.m_ints;
        material.m_vector2s    = data.m_vector2s;
        material.m_vector3s    = data.m_vector3s;
        material.m_vector4s    = data.m_vector4s;
        material.m_matrices    = data.m_matrices;

        for (std::map<std::string, ShaderSamplerData>::iterator it = data.m_sampler2Ds.begin(); it != data.m_sampler2Ds.end(); ++it)
        {
            material.m_sampler2Ds[it->first] = {it->second.m_unit, nullptr, "", "", it->second.m_bindMode, false};
        }

        return material;
    }

    void Material::SetMaterialContainers(Material& material)
    {
        if (material.m_usesHDRI)
            s_hdriMaterials.emplace(&material);

        if (material.m_isShadowMapped)
            s_shadowMappedMaterials.emplace(&material);
    }

    void Material::UnloadAll()
    {
        s_loadedMaterials.clear();
        s_hdriMaterials.clear();
        s_shadowMappedMaterials.clear();
    }

    bool Material::MaterialExists(StringIDType id)
    {
        if (id < 0)
            return false;
        return !(s_loadedMaterials.find(id) == s_loadedMaterials.end());
    }

    bool Material::MaterialExists(const std::string& path)
    {
        return MaterialExists(StringID(path.c_str()).value());
    }

    void Material::UnloadMaterialResource(StringIDType id)
    {
        if (!MaterialExists(id))
        {
            LINA_WARN("Material not found! Aborting... ");
            return;
        }

        // If its in the internal list, remove first.
        if (s_shadowMappedMaterials.find(&s_loadedMaterials[id]) != s_shadowMappedMaterials.end())
            s_shadowMappedMaterials.erase(&s_loadedMaterials[id]);

        s_loadedMaterials.erase(id);
    }
} // namespace Lina::Graphics
