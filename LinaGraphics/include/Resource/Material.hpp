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

#pragma once

#ifndef Material_HPP
#define Material_HPP

#define VERSION_MATERIAL 1

#include "Core/IResource.hpp"
#include "Core/ResourceHandle.hpp"
#include "Shader.hpp"
#include <cereal/access.hpp>

namespace Lina
{
    namespace Event
    {
        struct EResourceLoaded;
    }
}

namespace Lina::Graphics
{
    class Material : public Resources::IResource
    {
    public:
        Material() = default;
        virtual ~Material();

        virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const String& path) override;

        inline Resources::ResourceHandle<Shader>& GetShaderHandle()
        {
            return m_shader;
        }

    private:
    
        void FindShader();
        void OnResourceLoaded(const Event::EResourceLoaded& ev);

    private:

        Resources::ResourceHandle<Shader> m_shader;

        friend class cereal::access;

        template <class Archive>
        void serialize(Archive& archive, std::uint32_t const version)
        {
            if (version == VERSION_MATERIAL)
                archive(m_shader);
            else
            {
                // Previous version
                // archive(m_dummy);
            }
        }
    };

} // namespace Lina::Graphics

CEREAL_CLASS_VERSION(Lina::Graphics::Material, VERSION_MATERIAL);

#endif
