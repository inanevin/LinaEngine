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

#include "Rendering/RenderingCommon.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Resources/ResourceStorage.hpp"
#include "Rendering/ShaderInclude.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Lina::Graphics
{

    char* g_materialSurfaceTypeStr[2]{"Opaque", "Transparent"};
    char* g_materialWorkflowTypeStr[2]{"Plastic", "Metallic"};

    bool LoadTextWithIncludes(std::string& output, const std::string& includeKeyword)
    {
        auto*              storage       = Resources::ResourceStorage::Get();
        auto&              includesCache = storage->GetCache<ShaderInclude>();
        std::istringstream iss(output);
        std::stringstream  ss;

        for (std::string line; std::getline(iss, line);)
        {
            if (line.find(includeKeyword) == std::string::npos)
                ss << line << "\n";
            else
            {
                std::string includeFileName = Utility::Split(line, ' ')[1];
                includeFileName             = includeFileName.substr(1, includeFileName.length() - 2);
                const std::string includeID = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(includeFileName));
                std::string       toAppend  = "";

                for (auto& incRes : includesCache)
                {
                    ShaderInclude*     include = storage->GetResource<ShaderInclude>(incRes.first);
                    const std::string& name    = Utility::GetFileNameOnly(Utility::GetFileWithoutExtension(include->GetPath()));

                    if (name.compare(includeID) == 0)
                    {
                        toAppend = include->GetText();
                        break;
                    }
                }

                ss << toAppend << "\n";
            }
        }

        output = ss.str();
        return true;
    }

    bool LoadTextFileWithIncludes(std::string& output, const std::string& fileName, const std::string& includeKeyword)
    {
        auto*         storage       = Resources::ResourceStorage::Get();
        auto&         includesCache = storage->GetCache<ShaderInclude>();
        std::ifstream file;
        file.open(fileName.c_str());
        std::string       filePath = Utility::GetFilePath(fileName);
        std::stringstream ss;
        std::string       line;

        if (file.is_open())
        {
            while (file.good())
            {
                getline(file, line);

                if (line.find(includeKeyword) == std::string::npos)
                    ss << line << "\n";

                else
                {
                    std::string includeFileName = Utility::Split(line, ' ')[1];
                    includeFileName             = includeFileName.substr(1, includeFileName.length() - 2);
                    const std::string includeID = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(includeFileName));

                    std::string toAppend = "";

                    for (auto& incRes : includesCache)
                    {
                        ShaderInclude*     include = storage->GetResource<ShaderInclude>(incRes.first);
                        const std::string& name    = Utility::GetFileNameOnly(Utility::GetFileWithoutExtension(include->GetPath()));

                        if (name.compare(includeID) == 0)
                        {
                            toAppend = include->GetText();
                            break;
                        }
                    }

                    ss << toAppend << "\n";
                }
            }
        }
        else
        {
            LINA_ERR("File could not be loaded! {0}", fileName);
            return false;
        }

        output = ss.str();
        return true;
    }

    int SamplerFilterToInt(SamplerFilter filter)
    {
        if (filter == SamplerFilter::FILTER_NEAREST)
            return 0;
        else if (filter == SamplerFilter::FILTER_LINEAR)
            return 1;
        else if (filter == SamplerFilter::FILTER_NEAREST_MIPMAP_NEAREST)
            return 2;
        else if (filter == SamplerFilter::FILTER_LINEAR_MIPMAP_NEAREST)
            return 3;
        else if (filter == SamplerFilter::FILTER_NEAREST_MIPMAP_LINEAR)
            return 4;
        else if (filter == SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR)
            return 5;

        return 0;
    }

    int WrapModeToInt(SamplerWrapMode wrapMode)
    {
        if (wrapMode == SamplerWrapMode::WRAP_CLAMP_EDGE)
            return 0;
        else if (wrapMode == SamplerWrapMode::WRAP_CLAMP_MIRROR)
            return 1;
        else if (wrapMode == SamplerWrapMode::WRAP_CLAMP_BORDER)
            return 2;
        else if (wrapMode == SamplerWrapMode::WRAP_REPEAT)
            return 3;
        else if (wrapMode == SamplerWrapMode::WRAP_REPEAT_MIRROR)
            return 4;

        return 0;
    }

    SamplerFilter SamplerFilterFromInt(int id)
    {
        if (id == 0)
            return SamplerFilter::FILTER_NEAREST;
        else if (id == 1)
            return SamplerFilter::FILTER_LINEAR;
        else if (id == 2)
            return SamplerFilter::FILTER_NEAREST_MIPMAP_NEAREST;
        else if (id == 3)
            return SamplerFilter::FILTER_LINEAR_MIPMAP_NEAREST;
        else if (id == 4)
            return SamplerFilter::FILTER_NEAREST_MIPMAP_LINEAR;
        else if (id == 5)
            return SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;

        return SamplerFilter::FILTER_LINEAR;
    }

    SamplerWrapMode WrapModeFromInt(int id)
    {
        if (id == 0)
            return SamplerWrapMode::WRAP_CLAMP_EDGE;
        else if (id == 1)
            return SamplerWrapMode::WRAP_CLAMP_MIRROR;
        else if (id == 2)
            return SamplerWrapMode::WRAP_CLAMP_BORDER;
        else if (id == 3)
            return SamplerWrapMode::WRAP_REPEAT;
        else if (id == 4)
            return SamplerWrapMode::WRAP_REPEAT_MIRROR;

        return SamplerWrapMode::WRAP_CLAMP_BORDER;
    }

    PixelFormat GetPixelFormatFromNumChannels(int num)
    {
        switch (num)
        {
        case 1:
            return PixelFormat::FORMAT_R;
        case 2:
            return PixelFormat::FORMAT_RG;
        case 3:
            return PixelFormat::FORMAT_RGB;
        case 4:
            return PixelFormat::FORMAT_RGBA;
        default:
            return PixelFormat::FORMAT_RGBA;
        }
    }

} // namespace Lina::Graphics
