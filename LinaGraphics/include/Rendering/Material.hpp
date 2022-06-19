/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

/*
Class: Material

Representation of a material. Materials are used by mesh renderers and hold all the shader
related data and data structures.

Timestamp: 4/26/2019 1:12:18 AM
*/

#pragma once

#ifndef Material_HPP
#define Material_HPP

#include "Rendering/RenderingCommon.hpp"
#include "Resources/IResource.hpp"
#include "Resources/ResourceHandle.hpp"
#include "Core/CommonReflection.hpp"
#include "Shader.hpp"
#include "Data/Map.hpp"
#include "Data/Serialization/MapSerialization.hpp"


namespace Lina::Graphics
{
    class RenderEngine;
    class Texture;
    class Shader;

    struct MaterialSampler2D
    {
        uint32                             m_unit     = 0;
        TextureBindMode                    m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
        bool                               m_isActive = false;
        Resources::ResourceHandle<Texture> m_texture;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_unit, m_texture, m_bindMode, m_isActive);
        }
    };

    LINA_CLASS("Material")
    class Material : public Resources::IResource
    {
    public:
        Material()          = default;
        virtual ~Material() = default;

        static Material* CreateMaterial(Shader* shader, const String& savePath);
        virtual void*    LoadFromFile(const String& path) override;
        virtual void*    LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        void             Save();
        void             SetShader(Shader* shader, bool onlySetID = false);
        void             UpdateMaterialData();
        void             PostLoadMaterialData();
        void             SetTexture(const String& textureName, Texture* texture, TextureBindMode bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D);
        void             RemoveTexture(const String& textureName);
        Texture*         GetTexture(const String& name);

        inline void SetFloat(const String& name, float value)
        {
            m_floats[name] = value;
        }

        inline void SetBool(const String& name, bool value)
        {
            m_bools[name] = value;
        }

        inline void SetColor(const String& name, const Color& color)
        {
            m_colors[name] = color;
        }

        inline void SetVector2(const String& name, const Vector2& vector)
        {
            m_vector2s[name] = vector;
        }

        inline void SetVector3(const String& name, const Vector3& vector)
        {
            m_vector3s[name] = vector;
        }

        inline void SetVector4(const String& name, const Vector4& vector)
        {
            m_vector4s[name] = vector;
        }

        inline void SetMatrix4(const String& name, const Matrix& matrix)
        {
            m_matrices[name] = matrix;
        }

        inline float GetFloat(const String& name)
        {
            return m_floats[name];
        }

        inline float GetBool(const String& name)
        {
            return m_bools[name];
        }

        inline int GetInt(const String& name)
        {
            return m_ints[name];
        }

        inline Color GetColor(const String& name)
        {
            return m_colors[name];
        }

        inline Vector2 GetVector2(const String& name)
        {
            return m_vector2s[name];
        }

        inline Vector3 GetVector3(const String& name)
        {
            return m_vector3s[name];
        }

        inline Vector4 GetVector4(const String& name)
        {
            return m_vector4s[name];
        }

        inline Matrix GetMatrix(const String& name)
        {
            return m_matrices[name];
        }

        void SetSurfaceType(MaterialSurfaceType type);
        void SetInt(const String& name, int value);

        inline Resources::ResourceHandle<Shader> GetShaderHandle()
        {
            return m_shaderHandle;
        }

        inline MaterialSurfaceType GetSurfaceType()
        {
            return m_surfaceType;
        }

        friend class cereal::access;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_environmentHDR, m_skyboxIndirectContributionFactor,
            m_shaderHandle,  m_surfaceType, m_sampler2Ds, m_floats, m_ints, m_colors, m_vector2s, m_vector3s, m_vector4s, m_matrices, m_bools
            );
        }

        Map<String, float>             m_floats;
        Map<String, int>               m_ints;
        Map<String, MaterialSampler2D> m_sampler2Ds;
        Map<String, Color>             m_colors;
        Map<String, Vector2>           m_vector2s;
        Map<String, Vector3>           m_vector3s;
        Map<String, Vector4>           m_vector4s;
        Map<String, Matrix>            m_matrices;
        Map<String, bool>              m_bools;
        bool                                     m_hdriDataSet       = false;
        bool                                     m_reflectionDataSet = false;
        bool                                     m_skyIrradianceDataSet = false;


        // SKYBOX MATERIAL PROPERTIES
        Resources::ResourceHandle<Graphics::Texture> m_environmentHDR;
        float m_skyboxIndirectContributionFactor = 1.0f;

        LINA_PROPERTY("Shader", "Shader")
        Resources::ResourceHandle<Shader> m_shaderHandle;

    private:
        friend class RenderEngine;

        MaterialSurfaceType m_surfaceType = MaterialSurfaceType::Opaque;
    };

} // namespace Lina::Graphics

#endif
