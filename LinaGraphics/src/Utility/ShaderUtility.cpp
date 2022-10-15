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

#include "Utility/ShaderUtility.hpp"
#include "Math/Matrix.hpp"
#include <sstream>
#include <iostream>
#include <string>

namespace Lina::Graphics
{
    Vector<Pair<String, size_t>> DataTypes = {
        {"vec2", sizeof(float) * 2},
        {"vec3", sizeof(float) * 3},
        {"vec4", sizeof(float) * 4},
        {"float", sizeof(float)},
        {"double", sizeof(double)},
        {"int", sizeof(int)},
        {"mat4", sizeof(Matrix)},
    };

    String ShaderUtility::GetShaderStageText(const String& shader, const String& defineStart)
    {
        std::istringstream f(shader.c_str());
        std::string        line;
        bool               append = false;
        String             out    = "";
        while (std::getline(f, line))
        {
            if (!line.empty() && *line.rbegin() == '\r')
                line.erase(line.end() - 1);

            if (append && line.compare("#LINA_END") == 0)
                break;

            if (append)
            {
                out.append(line.c_str());
                out.append("\n");
            }

            if (line.compare(defineStart.c_str()) == 0)
                append = true;
        }
        return out;
    }

    Vector<PushConstantRange> ShaderUtility::CheckForPushConstants(ShaderStage stage, const String& moduleText)
    {
        Vector<PushConstantRange> v;
        std::istringstream        f(moduleText.c_str());
        std::string               line      = "";
        bool                      parsingPC = false;
        PushConstantRange         lastRange = PushConstantRange{
                    .stageFlags = GetShaderStage(ShaderStage::Vertex),
        };

        while (std::getline(f, line))
        {
            if (!line.empty() && *line.rbegin() == '\r')
                line.erase(line.end() - 1);

            if (line.find("push_constant") != std::string::npos)
                parsingPC = true;

            if (parsingPC)
            {
                for (auto& p : DataTypes)
                {
                    if (line.find(p.first.c_str()) != std::string::npos)
                        lastRange.size += static_cast<uint32>(p.second);
                }

                if (line.find("}") != std::string::npos)
                {
                    v.push_back(lastRange);
                    parsingPC = false;
                }
            }
        }

        return v;
    }
    Vector<ShaderDescriptorSetInfo> ShaderUtility::CheckForDescriptorSets(uint32 stageFlags, const String& moduleText)
    {
        Vector<ShaderDescriptorSetInfo> v;

        std::istringstream f(moduleText.c_str());
        std::string        line = "";

        ShaderDescriptorSetInfo lastSetInfo;
        bool                    parsingBinding = false;

        while (std::getline(f, line))
        {
            if (line.find("//") != std::string::npos)
                continue;

            std::string::iterator end_pos = std::remove(line.begin(), line.end(), ' ');
            line.erase(end_pos, line.end());

            if (!line.empty() && *line.rbegin() == '\r')
                line.erase(line.end() - 1);

            if (!parsingBinding && line.find("set") != std::string::npos)
            {
                size_t      set         = line.find("set=");
                size_t      firstComma  = line.find(",");
                std::string setIndexStr = line.substr(set + 4, firstComma - set - 4);

                size_t      binding         = line.find("binding=");
                size_t      paranth         = line.find(")");
                std::string bindingIndexStr = line.substr(binding + 8, paranth - binding - 8);

                uint32 setIndex          = static_cast<uint32>(std::stoi(setIndexStr));
                uint32 bindingIndex      = static_cast<uint32>(std::stoi(bindingIndexStr));
                lastSetInfo.bindingIndex = bindingIndex;
                lastSetInfo.stageFlags   = stageFlags;
                lastSetInfo.setIndex     = setIndex;

                if (line.find("sampler2D") != std::string::npos)
                    lastSetInfo.type = DescriptorType::CombinedImageSampler;
                else if (line.find("readonly buffer") != std::string::npos)
                    lastSetInfo.type = DescriptorType::StorageBuffer;
                else
                    lastSetInfo.type = DescriptorType::UniformBuffer;

                parsingBinding = true;
            }

            if (parsingBinding)
            {
                if (line.find("}") != std::string::npos)
                {
                    v.push_back(lastSetInfo);
                    parsingBinding = false;
                }
            }
        }

        return v;
    }

    Bitmask16 ShaderUtility::GetPassMask(const String& text)
    {
        Bitmask16 mask;

        std::istringstream f(text.c_str());
        std::string        line = "";

        while (std::getline(f, line))
        {
            if (line.find("//") != std::string::npos)
                continue;

            if (!line.empty() && *line.rbegin() == '\r')
                line.erase(line.end() - 1);

            if (line.compare("#LINA_PASS_OPAQUE") == 0)
                mask.Set(DrawPassMask::Opaque);
            else if (line.compare("#LINA_PASS_TRANSPARENT") == 0)
                mask.Set(DrawPassMask::Transparent);
            else if (line.compare("#LINA_PASS_SHADOWS") == 0)
                mask.Set(DrawPassMask::Shadow);
        }

        return mask;
    }

    void ShaderUtility::FillMaterialProperties(const String& text, HashMap<uint8, Vector<String>>& map)
    {

        std::istringstream f(text.c_str());
        std::string        line            = "";
        bool               readingMaterial = false;

        std::vector<std::string> types;
        types.push_back("float");
        types.push_back("int");
        types.push_back("bool");
        types.push_back("vec2");
        types.push_back("vec4");

        const int typesSize = static_cast<uint32>(types.size());
        while (std::getline(f, line))
        {
            if (line.find("//") != std::string::npos)
                continue;

            if (!line.empty() && *line.rbegin() == '\r')
                line.erase(line.end() - 1);

            if (line.find("uniform MaterialData") != std::string::npos)
                readingMaterial = true;

            if (readingMaterial)
            {
                if (line.find("{") != std::string::npos)
                    continue;

                size_t type = std::string::npos;
                uint8 i    = 0;

                while (type == std::string::npos && i < typesSize)
                {
                    type = line.find(types[i]);

                    if (type != std::string::npos)
                    {
                        const size_t varNameStart = type + types[i].size() + 1;
                        String       varName      = line.substr(varNameStart, line.size() - varNameStart - 1).c_str();
                        map[i].push_back(varName);
                    }

                    i++;
                }
            }

            if (readingMaterial && line.find("}") != std::string::npos)
            {
                break;
            }
        }
    }

    void ShaderUtility::FillRenderPasses(const String& text, Vector<uint8>& vec)
    {
        std::istringstream f(text.c_str());
        std::string        line = "";

        while (std::getline(f, line))
        {
            if (line.find("//") != std::string::npos)
                continue;

            if (!line.empty() && *line.rbegin() == '\r')
                line.erase(line.end() - 1);

            if (line.find("#LINA_RP_MAIN") != std::string::npos)
            {
                vec.push_back(static_cast<uint8>(RenderPassType::Main));
                continue;
            }
            else if (line.find("#LINA_RP_PP") != std::string::npos)
            {
                vec.push_back(static_cast<uint8>(RenderPassType::PostProcess));
                continue;
            }
            else if (line.find("#LINA_RP_FINAL") != std::string::npos)
            {
                vec.push_back(static_cast<uint8>(RenderPassType::Final));
                continue;
            }
        }
    }
} // namespace Lina::Graphics
