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

#include "Resources/ResourceStorage.hpp"

namespace Lina::Resources
{
    ResourceStorage* ResourceStorage::s_instance = nullptr;

    ResourceStorage::~ResourceStorage()
    {
        for (auto& p : m_resources)
        {
            for (auto& rp : p.second)
                delete rp.second;

            p.second.clear();
        }

        m_resources.clear();
    }
    TypeID ResourceStorage::GetTypeIDFromExtension(const std::string& extension)
    {
        bool   found = false;
        TypeID tid   = -1;

        for (auto& p : m_resourceTypes)
        {
            auto& extensions = p.second.m_associatedExtensions;

            for (int i = 0; i < extensions.size(); i++)
            {
                if (extensions[i].compare(extension) == 0)
                {
                    tid   = p.first;
                    found = true;
                    break;
                }
            }

            if (found)
                break;
        }

        return tid;
    }

    bool ResourceStorage::IsPriorityResource(TypeID tid)
    {
        for (int i = 0; i < m_priorityResourceTypes.size(); i++)
        {
            if (m_priorityResourceTypes[i] == tid)
                return true;
        }

        return false;
    }
} // namespace Lina::Resources