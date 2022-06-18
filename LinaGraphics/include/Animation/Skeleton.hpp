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
Class: Skeleton



Timestamp: 12/7/2021 2:13:56 PM
*/

#pragma once

#ifndef Skeleton_HPP
#define Skeleton_HPP

// Headers here.
#include "Rendering/RenderingCommon.hpp"

#include <map>
#include <string>

namespace Lina::Graphics
{
    class Animation;

    class Skeleton
    {

    public:
        Skeleton() = default;
        ~Skeleton();

        bool LoadSkeleton(const std::string& path);

        bool IsLoaded() const
        {
            return m_loaded;
        }

        void SetVertexBoneDataToDefault(VertexBoneData& vertex);
        void SetVertexBoneData(VertexBoneData& vertex, int boneID, float weight);

        std::map<std::string, Animation*>& GetAnimations()
        {
            return m_animationMap;
        }

    private:
        bool                              m_loaded = false;
        Vector<VertexBoneData>       m_vertexBoneData;
        std::map<std::string, Animation*> m_animationMap;
        std::map<std::string, BoneInfo>   m_boneInfoMap;

        int   m_boneIDs[MAX_BONE_INFLUENCE];
        float m_boneWeights[MAX_BONE_INFLUENCE];
    };
} // namespace Lina::Graphics

#endif
