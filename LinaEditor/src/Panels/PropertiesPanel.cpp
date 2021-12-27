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

#include "Panels/PropertiesPanel.hpp"

#include "Core/Application.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/GUILayer.hpp"
#include "Drawers/EntityDrawer.hpp"
#include "IconsFontAwesome5.h"
#include "Rendering/Model.hpp"
#include "Utility/EditorUtility.hpp"
#include "Widgets/WidgetsUtility.hpp"

namespace Lina::Editor
{
    using namespace ECS;
    using namespace Lina;
    static bool s_openCompExistsModal;

    void PropertiesPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);
        Event::EventSystem::Get()->Connect<EEntitySelected, &PropertiesPanel::EntitySelected>(this);
        Event::EventSystem::Get()->Connect<EModelSelected, &PropertiesPanel::ModelSelected>(this);
        //	Event::EventSystem::Get()->Connect<EMaterialSelected, &PropertiesPanel::MaterialSelected>(this);
        Event::EventSystem::Get()->Connect<ETextureSelected, &PropertiesPanel::TextureSelected>(this);
        Event::EventSystem::Get()->Connect<EEntityUnselected, &PropertiesPanel::Unselect>(this);

        m_entityDrawer.Initialize();
    }

    void PropertiesPanel::EntitySelected(const EEntitySelected& ev)
    {
        m_entityDrawer.SetSelectedEntity(ev.m_entity);
        m_currentDrawType = DrawType::Entities;
    }

    void PropertiesPanel::TextureSelected(const ETextureSelected& ev)
    {
        // m_textureDrawer.SetSelectedTexture(ev.m_texture);
        m_currentDrawType = DrawType::Texture2D;
    }

    // void PropertiesPanel::MaterialSelected(EMaterialSelected ev)
    // {
    // 	m_materialDrawer.SetSelectedMaterial(ev.m_file, *ev.m_material);
    // 	m_currentDrawType = DrawType::Material;
    // }
    void PropertiesPanel::ModelSelected(const EModelSelected& ev)
    {
        // m_modelDrawer.SetSelectedModel(*ev.m_model);
        m_currentDrawType = DrawType::Model;
    }

    void PropertiesPanel::Draw()
    {
        if (m_show)
        {
            Begin();

            // Draw the selected item.
            if (m_currentDrawType == DrawType::Entities)
                m_entityDrawer.DrawSelectedEntity();
            // else if (m_currentDrawType == DrawType::Texture2D)
            //	m_textureDrawer.DrawSelectedTexture();
            // else if (m_currentDrawType == DrawType::Model)
            //	m_modelDrawer.DrawSelectedMesh();
            // else if (m_currentDrawType == DrawType::Material)
            //	m_materialDrawer.DrawSelectedMaterial();

            End();
        }
    }

    void PropertiesPanel::Unselect(const EEntityUnselected& ev)
    {
        m_currentDrawType = DrawType::None;
    }

} // namespace Lina::Editor
