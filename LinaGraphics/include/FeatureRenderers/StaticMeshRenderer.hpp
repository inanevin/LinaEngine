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

#ifndef StaticMeshRenderer_HPP
#define StaticMeshRenderer_HPP

#include "Data/HashSet.hpp"
#include "Data/Vector.hpp"
#include "Components/ModelNodeComponent.hpp"
#include "Core/RenderData.hpp"
#include "Data/DataCommon.hpp"

namespace Lina
{
    namespace World
    {
        class EntityWorld;
        class Entity;
    } // namespace World
} // namespace Lina

namespace Lina::Graphics
{
    class View;
    class Material;
    class ModelNode;
    class Mesh;
    class FramePacket;
    class ModelNodeComponent;
    class FeatureRendererManager;

    class StaticMeshRenderer
    {
    public:
        struct RenderPair
        {
            Matrix transform = Matrix::Identity();
            Mesh*  mesh      = nullptr;
        };

        struct Data
        {
            HashSet<VisibilityData>                m_visibleObjects;
            HashSet<ModelNodeComponent*>           m_visibleNodeComponents;
            HashMap<Material*, Vector<RenderPair>> m_gpuData;
        };

        void Initialize(FeatureRendererManager& manager);
        void Shutdown();

        void OnFetchVisibility(World::EntityWorld* world);
        void OnAssignVisibility(FramePacket& fp);
        void OnExtractPerView(View* v);
        void OnPrepare();
        void OnSubmitPerView(CommandBuffer& buffer, View* v);

        Data& GetDataToWrite()
        {
            return m_renderData[m_dataToWrite];
        }

        Data& GetDataToRead()
        {
            return m_renderData[m_dataToRead];
        }

    private:
        Data m_renderData[2];
        int  m_dataToWrite = 0;
        int  m_dataToRead  = 1;
    };
} // namespace Lina::Graphics

#endif
