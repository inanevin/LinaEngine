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

/*
Class: PropertiesPanel

The central system for drawing properties of any objects & files including materials, textures,
meshes, entities etc.

Timestamp: 6/7/2020 5:13:24 PM
*/

#pragma once

#ifndef PropertiesPanel_HPP
#define PropertiesPanel_HPP

#include "Core/EditorCommon.hpp"
#include "Drawers/EntityDrawer.hpp"
#include "Panels/EditorPanel.hpp"

namespace Lina
{
    namespace Graphics
    {
        class Model;
        class Material;
    } // namespace Graphics
} // namespace Lina

namespace Lina::Editor
{
    class EditorFile;
    struct EEntitySelected;
    struct ETextureSelected;
    struct EModelSelected;
    struct EEntityUnselected;

    class PropertiesPanel : public EditorPanel
    {

        enum class DrawType
        {
            None,
            Entities,
            Texture2D,
            Model,
            Material
        };

    public:
        PropertiesPanel() = default;
        virtual ~PropertiesPanel() = default;

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;

        void EntitySelected(const EEntitySelected& ev);
        void TextureSelected(const ETextureSelected& ev);
        void ModelSelected(const EModelSelected& ev);
        void Unselect(const EEntityUnselected& ev);

    private:
        // Selected entity.
        EntityDrawer m_entityDrawer;

        DrawType m_currentDrawType = DrawType::None;
    };
} // namespace Lina::Editor

#endif
