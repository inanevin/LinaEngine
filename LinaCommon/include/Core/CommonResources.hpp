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
Class: CommonResources



Timestamp: 12/25/2021 11:44:29 AM
*/

#pragma once

#ifndef CommonResources_HPP
#define CommonResources_HPP

// Headers here.
#include "Utility/StringId.hpp"
#include "Math/Color.hpp"

#include <string>
#include <unordered_map>

namespace Lina::Resources
{
    enum class ResourceType
    {
        Unknown         = 0,
        Model           = 1,
        ModelAssetData  = 2,
        Image           = 3,
        ImageData       = 4,
        HDR             = 5,
        Audio           = 6,
        AudioData       = 7,
        Material        = 8,
        GLSL            = 9,
        GLH             = 10,
        SPIRV           = 11,
        Font            = 12,
        PhysicsMaterial = 13,
        Folder          = 14,
        UserAsset       = 20
    };

    enum class ResourceProgressState
    {
        None,
        Pending,
        InProgress
    };

    class ResourceProgressData
    {
    public:
        ResourceProgressData()  = default;
        ~ResourceProgressData() = default;

        ResourceProgressState m_state                 = ResourceProgressState::None;
        std::string           m_currentResourceName   = "";
        std::string           m_progressTitle         = "";
        float                 m_currentProgress       = 0.0f;
        int                   m_currentTotalFiles     = 0;
        int                   m_currentProcessedFiles = 0;

    private:
        ResourceProgressData(const ResourceProgressData&);            // = delete
        ResourceProgressData& operator=(const ResourceProgressData&); // = delete
    };

    /// <summary>
    /// Register a user defined resource to be picked-up & request to load by the resource manager.
    /// Pass in the extension without '.'
    /// </summary>
    extern void                                           RegisterUserDefinedResource(const std::string& extension);
    extern std::unordered_map<StringIDType, ResourceType> m_resourceTypeTable;
    extern ResourceType                                   GetResourceType(const std::string& extension);
    extern Color                                          GetResourceTypeColor(ResourceType type);
} // namespace Lina::Resources

#endif
