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

#include "Animation/Skeleton.hpp"
#include "Utility/Log.hpp"
#include <ozz/animation/runtime/animation.h>
#include <ozz/base/io/stream.h>
#include <ozz/base/io/archive.h>
namespace Lina::Graphics
{
    Skeleton::~Skeleton()
    {
        for (std::map<std::string, Animation*>::iterator it = m_animationMap.begin(); it != m_animationMap.end(); it++)
        {
            delete it->second;
        }

        m_animationMap.clear();
    }

    bool Skeleton::LoadSkeleton(const std::string& skelPath)
    {
        LINA_CORE_TRACE("Loading skeleton archive: {0}", skelPath);

        return true;
    }
    void Skeleton::SetVertexBoneDataToDefault(VertexBoneData& vertex)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            vertex.m_boneIDs[i] = -1;
            vertex.m_boneWeights[i] = 0.0f;
        }
    }

    void Skeleton::SetVertexBoneData(VertexBoneData& vertex, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            if (vertex.m_boneIDs[i] < 0)
            {
                vertex.m_boneWeights[i] = weight;
                vertex.m_boneIDs[i] = boneID;
                break;
            }
        }
    }
}