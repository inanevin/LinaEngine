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

#include "Rendering/Shader.hpp"

#include "Core/RenderEngine.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace Lina::Graphics
{

    Shader::~Shader()
    {
        m_engineBoundID = m_renderDevice->ReleaseShaderProgram(m_engineBoundID);
    }

    void* Shader::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        LINA_TRACE("[Shader Loader - Memory] -> Loading: {0}", path);
        Resources::IResource::SetSID(path);
        String shaderText = String(reinterpret_cast<char*>(data), dataSize);
        Graphics::LoadTextWithIncludes(shaderText, "#include");
        const bool usesGeometryShader = shaderText.find("GS_BUILD") != String::npos;
        this->Construct(shaderText, usesGeometryShader);
        return static_cast<void*>(this);
    }

    void* Shader::LoadFromFile(const String& path)
    {
        LINA_TRACE("[Shader Loader - File] -> Loading: {0}", path);
        Resources::IResource::SetSID(path);
        String shaderText;
        Graphics::LoadTextFileWithIncludes(shaderText, path, "#include");
        const bool usesGeometryShader = shaderText.find("GS_BUILD") != String::npos;
        this->Construct(shaderText, usesGeometryShader);
        return static_cast<void*>(this);
    }

    Shader& Shader::Construct(const String& text, bool usesGeometryShader)
    {
        m_specification = CheckForSpecification(text);
        m_renderDevice  = RenderEngine::Get()->GetRenderDevice();
        m_engineBoundID = m_renderDevice->CreateShaderProgram(text, &m_uniformData, usesGeometryShader);
        return *this;
    }

    void Shader::SetUniformBuffer(const String& name, UniformBuffer& buffer)
    {
        m_renderDevice->SetShaderUniformBuffer(m_engineBoundID, name, buffer.GetID());
    }

    void Shader::BindBlockToBuffer(uint32 bindingPoint, String blockName)
    {
        m_renderDevice->BindShaderBlockToBufferPoint(m_engineBoundID, bindingPoint, blockName);
    }

    ShaderSpecification Shader::CheckForSpecification(const String& text)
    {
        if (text.find("#define LINA_SKY_CUBE") != String::npos)
            return ShaderSpecification::Sky_Cube;
        else if (text.find("#define LINA_SKY_HDRICUBE") != String::npos)
            return ShaderSpecification::Sky_HDRICube;
        else if (text.find("#define LINA_SKY_DOME") != String::npos)
            return ShaderSpecification::Sky_Dome;

        return ShaderSpecification::None;
    }

} // namespace Lina::Graphics
