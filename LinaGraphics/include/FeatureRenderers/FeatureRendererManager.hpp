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

#ifndef FeatureRendererManager_HPP
#define FeatureRendererManager_HPP

#include "Data/HashMap.hpp"
#include "Data/Vector.hpp"
#include "Functional/Functional.hpp"

namespace Lina
{
    namespace World
    {
        class EntityWorld;
    }
} // namespace Lina

namespace Lina::Graphics
{
    class View;
    class CommandBuffer;
    class FramePacket;

    typedef Delegate<void(World::EntityWorld* w)>          WorldFunction;
    typedef Delegate<void(World::EntityWorld* w, View* v)> WorldViewFunction;
    typedef Delegate<void(View* v)>                        ViewFunction;
    typedef Delegate<void(CommandBuffer& cb, View* v)>     CBFunction;
    typedef Delegate<void()>                               DefFunction;

    class CommandBuffer;

    class FeatureRendererManager
    {
    public:
        Vector<WorldFunction>     onExtract;
        Vector<WorldViewFunction> onExtractPerView;
        Vector<WorldViewFunction> onExtractPerViewEnd;
        Vector<WorldFunction>     onExtractEnd;
        Vector<DefFunction>       onPrepare;
        Vector<ViewFunction>      onPreparePerView;
        Vector<ViewFunction>      onPreparePerViewEnd;
        Vector<DefFunction>       onPrepareEnd;
        Vector<CBFunction>        onSubmit;

    private:
        friend class Renderer;

        void ExtractGameState(World::EntityWorld* world);
        void PrepareRenderData();
        void Submit(CommandBuffer& buf);

    private:
        Renderer* m_renderer = nullptr;
    };
}; // namespace Lina::Graphics
   // namespace Lina::Graphics

#endif
