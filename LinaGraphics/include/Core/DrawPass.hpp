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

#ifndef DrawPass_HPP
#define DrawPass_HPP

#include "GraphicsCommon.hpp"
#include "Data/Mutex.hpp"
#include "RenderData.hpp"
#include "PipelineObjects/Buffer.hpp"

namespace Lina
{
    namespace World
    {
        class Entity;
    }
} // namespace Lina

namespace Lina::Graphics
{
    class View;
    class RenderableComponent;
    class RenderableList;
    class CommandBuffer;
    class Material;
    class Mesh;

    class DrawPass
    {
    public:
        void PrepareRenderData(Vector<RenderableData>& drawList);
        void UpdateViewData(Buffer& viewDataBuffer);
        void RecordDrawCommands(CommandBuffer& cmd, RenderPassType rpType);

    private:
        void ExtractPassRenderables(Vector<RenderableData>& drawList);
        void BatchPassRenderables();

        Vector<RenderableData>& GetVisibleRenderables()
        {
            return m_renderables;
        }

    private:
        void Initialize(DrawPassMask mask, View* view, float distance)
        {
            m_drawDistance = distance;
            m_passMask     = mask;
            m_view         = view;
        }

        int32 FindInBatches(const MeshMaterialPair& pair);

    private:
        friend class Renderer;

        Mutex                  m_mtx;
        Vector<RenderableData> m_renderables;
        Vector<InstancedBatch> m_batches;
        DrawPassMask           m_passMask     = DrawPassMask::Opaque;
        float                  m_drawDistance = 0.0f;
        View*                  m_view         = nullptr;
    };
} // namespace Lina::Graphics

#endif
