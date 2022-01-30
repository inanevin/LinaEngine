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
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <set>

namespace Lina::Graphics
{
    class OpenGLRenderEngine;
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

        static Material* CreateMaterial(Shader* shader, const std::string& savePath);
        virtual void*    LoadFromFile(const std::string& path) override;
        virtual void*    LoadFromMemory(const std::string& path, unsigned char* data, size_t dataSize) override;
        void             Save();
        void             SetShader(Shader* shader, bool onlySetID = false);
        void             UpdateMaterialData();
        void             PostLoadMaterialData();
        void             SetTexture(const std::string& textureName, Texture* texture, TextureBindMode bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D);
        void             RemoveTexture(const std::string& textureName);
        Texture*         GetTexture(const std::string& name);

        inline void SetFloat(const std::string& name, float value)
        {
            m_floats[name] = value;
        }

        inline void SetBool(const std::string& name, bool value)
        {
            m_bools[name] = value;
        }

        inline void SetColor(const std::string& name, const Color& color)
        {
            m_colors[name] = color;
        }

        inline void SetVector2(const std::string& name, const Vector2& vector)
        {
            m_vector2s[name] = vector;
        }

        inline void SetVector3(const std::string& name, const Vector3& vector)
        {
            m_vector3s[name] = vector;
        }

        inline void SetVector4(const std::string& name, const Vector4& vector)
        {
            m_vector4s[name] = vector;
        }

        inline void SetMatrix4(const std::string& name, const Matrix& matrix)
        {
            m_matrices[name] = matrix;
        }

        inline float GetFloat(const std::string& name)
        {
            return m_floats[name];
        }

        inline float GetBool(const std::string& name)
        {
            return m_bools[name];
        }

        inline int GetInt(const std::string& name)
        {
            return m_ints[name];
        }

        inline Color GetColor(const std::string& name)
        {
            return m_colors[name];
        }

        inline Vector2 GetVector2(const std::string& name)
        {
            return m_vector2s[name];
        }

        inline Vector3 GetVector3(const std::string& name)
        {
            return m_vector3s[name];
        }

        inline Vector4 GetVector4(const std::string& name)
        {
            return m_vector4s[name];
        }

        inline Matrix GetMatrix(const std::string& name)
        {
            return m_matrices[name];
        }

        void SetSurfaceType(MaterialSurfaceType type);
        void SetInt(const std::string& name, int value);

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
            archive(m_triggersHDRIReflections, m_receivesEnvironmentReflections, m_shaderHandle, m_surfaceType, m_sampler2Ds, m_floats, m_ints, m_colors, m_vector2s, m_vector3s, m_vector4s, m_matrices, m_bools);
        }

        std::map<std::string, float>             m_floats;
        std::map<std::string, int>               m_ints;
        std::map<std::string, MaterialSampler2D> m_sampler2Ds;
        std::map<std::string, Color>             m_colors;
        std::map<std::string, Vector2>           m_vector2s;
        std::map<std::string, Vector3>           m_vector3s;
        std::map<std::string, Vector4>           m_vector4s;
        std::map<std::string, Matrix>            m_matrices;
        std::map<std::string, bool>              m_bools;
        bool                                     m_hdriDataSet       = false;
        bool                                     m_reflectionDataSet = false;

        LINA_PROPERTY("Trigger HDRI", "Bool", "For Skybox materials, check if the material reflects the currently set environment map in render settings.")
        bool m_triggersHDRIReflections = false;

        LINA_PROPERTY("Receive Reflections", "Bool", "The material's shader will receive area-based reflection data if true.")
        bool m_receivesEnvironmentReflections = false;

        LINA_PROPERTY("Shader", "Shader")
        Resources::ResourceHandle<Shader> m_shaderHandle;

    private:
        friend class OpenGLRenderEngine;

        MaterialSurfaceType m_surfaceType = MaterialSurfaceType::Opaque;
    };

} // namespace Lina::Graphics

#endif
