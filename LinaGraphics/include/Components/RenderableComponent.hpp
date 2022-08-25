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

#ifndef RenderableComponent_HPP
#define RenderableComponent_HPP

#include "Core/Component.hpp"
#include "Core/CommonReflection.hpp"

namespace Lina::Graphics
{
    LINA_COMPONENT("Renderable", "Graphics", "True")
    class RenderableComponent : public World::Component
    {
    public:
        virtual void OnComponentCreated() override;
        virtual void OnComponentDestroyed() override;

        virtual AABB& GetAABB() = 0;

        virtual bool IsVisible()
        {
            return enabled && isVisible;
        }

        virtual void SaveToArchive(cereal::PortableBinaryOutputArchive& oarchive) override
        {
            Component::SaveToArchive(oarchive);
            oarchive(dummy, dummy2);
        };

        virtual void LoadFromArchive(cereal::PortableBinaryInputArchive& iarchive) override
        {
            Component::LoadFromArchive(iarchive);
            iarchive(dummy, dummy2);
        };

        inline uint32 GetRenderableID()
        {
            return m_renderableID;
        }

        bool isVisible = true;
        int  dummy     = 0;
        int  dummy2    = 0;

    private:
        friend class Renderer;
        uint32 m_renderableID = 0;
    };
} // namespace Lina::Graphics

#endif
