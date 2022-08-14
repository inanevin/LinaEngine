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
#include "ECS/Registry.hpp"
#include <functional>

namespace Lina::Graphics
{
    class View;
    class CommandBuffer;

    typedef std::function<void()>                  FeatureRendererEmptyFunction;
    typedef std::function<void(View* v)>           FeatureRendererViewFunction;
    typedef std::function<void(CommandBuffer& cb)> FeatureRendererCBFunction;

    enum class FeatureRendererDefaultStage
    {
        OnExtract,
        OnExtractEnd,
        OnPrepare,
        OnPrepareEnd,
    };

    enum class FeatureRendererSubmitStage
    {
        OnSubmit,
    };

    enum class FeatureRendererVisibilityStage
    {
        FetchVisibility,
        AssignVisibility,
    };

    enum class FeatureRendererViewStage
    {
        OnExtractView,
        OnExtractViewFinalize,
        OnPrepareView,
        OnPrepareViewFinalize,
    };

    class CommandBuffer;

    class FeatureRendererManager
    {
    public:
        void RegisterDefaultStage(FeatureRendererDefaultStage stage, FeatureRendererEmptyFunction&& func);
        void RegisterViewStage(FeatureRendererViewStage stage, FeatureRendererViewFunction&& func);
        void RegisterVisibility(FeatureRendererVisibilityStage stage, FeatureRendererEmptyFunction&& func);
        void RegisterSubmit(FeatureRendererSubmitStage stage, FeatureRendererCBFunction&& func);

    private:
        friend class Renderer;

        void FetchVisibility();
        void ExtractGameState();
        void PrepareRenderData();
        void Submit(CommandBuffer& buf);

        // void OnExtract();
        // void OnExtractView(View* v);
        // void OnExtractViewsFinalize();
        // void OnPrepare();
        // void OnPrepareView(View* v);
        // void OnPrepareViewsFinalize();
        // void OnSubmit();

    private:
        HashMap<FeatureRendererDefaultStage, Vector<FeatureRendererEmptyFunction>>    m_defaultStages;
        HashMap<FeatureRendererViewStage, Vector<FeatureRendererViewFunction>>        m_viewStages;
        HashMap<FeatureRendererVisibilityStage, Vector<FeatureRendererEmptyFunction>> m_visibilityStages;
        HashMap<FeatureRendererSubmitStage, Vector<FeatureRendererCBFunction>>        m_submitStages;
    };
}; // namespace Lina::Graphics
   // namespace Lina::Graphics

#endif
