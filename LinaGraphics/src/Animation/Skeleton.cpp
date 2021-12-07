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
namespace LinaEngine::Graphics
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

        ozz::io::File file(skelPath.c_str(), "rb");
        if (!file.opened())
        {
            LINA_CORE_ERR("Failed to open skeleton file {0}", skelPath);
            return false;
        }

        ozz::io::IArchive archive(&file);
        if (!archive.TestTag<ozz::animation::Skeleton>())
        {
            LINA_CORE_ERR("Failed to load skeleton instance from file {0}", skelPath);
            return false;
        }

        // Once the tag is validated, reading cannot fail.
        archive >> m_skeleton;

        // Allocates runtime buffers.
        const int num_soa_joints = m_skeleton.num_soa_joints();
        m_locals.resize(num_soa_joints);
        const int num_joints = m_skeleton.num_joints();
        m_models.resize(num_joints);
        
        // Allocates a cache that matches animation requirements.
        m_cache.Resize(num_joints);

        m_loaded = true;
        return true;
    }
}