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

#include "Core/RenderableList.hpp"
#include "Core/Entity.hpp"
#include "Components/RenderableComponent.hpp"

namespace Lina::Graphics
{
#define LIST_STEP 250

    void RenderableList::Create()
    {
        m_renderables.resize(LIST_STEP, nullptr);
    }

    void RenderableList::Reset()
    {
        m_availableIDs = Queue<uint32>();
        m_nextFreeID   = 0;
        m_renderables.clear();
        Create();
    }

    void RenderableList::AddRenderable(RenderableComponent* comp)
    {
        uint32 id = 0;

        if (!m_availableIDs.empty())
        {
            id = m_availableIDs.front();
            m_availableIDs.pop();
        }
        else
            id = m_nextFreeID++;

        const uint32 currentSize = static_cast<uint32>(m_renderables.size());
        if (id >= currentSize)
            m_renderables.resize(LIST_STEP + currentSize);

        comp->m_renderableID = id;
        m_renderables[id]    = comp;
    }

    void RenderableList::RemoveRenderable(RenderableComponent* comp)
    {
        const uint32 id = comp->GetRenderableID();
        m_availableIDs.push(id);
        m_renderables[id] = nullptr;
    }

    RenderableComponent* RenderableList::EntityExists(World::Entity* entity)
    {
        for (auto& r : m_renderables)
        {
            if (r != nullptr)
            {
                if (r->GetEntity() == entity)
                    return r;
            }
        }

        return nullptr;
    }
} // namespace Lina::Graphics
