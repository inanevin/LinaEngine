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
Class: Shader

Wrapper for shader functionalities, responsible for creating shaders, binding uniform buffers
and all related shader operations.

Timestamp: 2/16/2019 1:47:28 AM
*/

#pragma once

#ifndef Shader_HPP
#define Shader_HPP

#include "Resources/IResource.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "Rendering/UniformBuffer.hpp"
#include <Data/String.hpp>

namespace Lina::Graphics
{
    enum class ShaderSpecification
    {
        None = 0,
        Sky_Dome = 1,
        Sky_Cube = 2,
        Sky_HDRICube = 3,
    };

    class Shader : public Resources::IResource
    {
    public:
        Shader() = default;
        ~Shader();

        virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const String& path) override;

        Shader& Construct(const String& text, bool usesGeometryShader);
        void    SetUniformBuffer(const String& name, UniformBuffer& buffer);
        void    BindBlockToBuffer(uint32 bindingPoint, String blockName);

        const ShaderUniformData& GetUniformData() const
        {
            return m_uniformData;
        }

        uint32 GetID()
        {
            return m_engineBoundID;
        }

        ShaderSpecification GetSpecification()
        {
            return m_specification;
        }

    private:
        ShaderSpecification CheckForSpecification(const String& text);

    private:
        ShaderUniformData   m_uniformData;
        RenderDevice*       m_renderDevice  = nullptr;
        uint32              m_engineBoundID = 0;
        ShaderSpecification m_specification = ShaderSpecification::None;
    };
} // namespace Lina::Graphics

#endif
