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

#include "Editor/Widgets/Docking/DockContainer.hpp"
#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/Docking/DockPreview.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{

    void DockContainer::Tick(float delta)
    {
        Widget::SetIsHovered();
        SetSize(m_parent->GetSize());
        Widget::Tick(delta);
    }
    
    void DockContainer::Draw(int32 threadIndex)
    {
        LinaVG::StyleOptions background;
        background.color = m_props.colorBackground.AsLVG4();
        LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), background, 0.0f, m_drawOrder);
        Widget::Draw(threadIndex);
    }

    void DockContainer::ShowPreview()
{
        LINA_ASSERT(m_preview == nullptr, "");
        m_preview = Allocate<DockPreview>("DockContainerPreview");
            
    }

    void DockContainer::HidePreview()
    {
        LINA_ASSERT(m_preview != nullptr, "");
        m_preview->Destroy();
        m_manager->Deallocate(m_preview);
    }

    DockArea* DockContainer::AddDockArea(DockDirection direction)
    {
        LINA_ASSERT(direction != DockDirection::Center, "");
        DockArea* area = Allocate<DockArea>();
        area->m_parentContainer = this;
        AddChild(area);
        
        // Setup areas percentages according to direction...
        if(direction == DockDirection::Left)
        {
            area->m_posAlign = Vector2::Zero;
            area->m_sizeAlign = Vector2(DEFAULT_DOCK_PERC, 1.0f);
        }
        else if(direction == DockDirection::Right)
        {
            area->m_posAlign = Vector2(1.0f - DEFAULT_DOCK_PERC, 0.0f);
            area->m_sizeAlign = Vector2(DEFAULT_DOCK_PERC, 1.0f);
        }
        else if(direction == DockDirection::Top)
        {
            area->m_posAlign = Vector2::Zero;
            area->m_sizeAlign = Vector2(1.0f, DEFAULT_DOCK_PERC);
        }
        else if(direction == DockDirection::Bottom)
        {
            area->m_posAlign = Vector2(0.0f, 1.0f - DEFAULT_DOCK_PERC);
            area->m_sizeAlign = Vector2(1.0f, DEFAULT_DOCK_PERC);
        }
    }
}
