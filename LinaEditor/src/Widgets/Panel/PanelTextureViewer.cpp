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

#include "Editor/Widgets/Panel/PanelTextureViewer.hpp"
#include "Editor/Editor.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/Graphics/Resource/Texture.hpp"

namespace Lina::Editor
{
	void PanelTextureViewer::Construct()
	{
		m_editor = Editor::Get();
        
        DirectionalLayout* horizontal = m_manager->Allocate<DirectionalLayout>("Horizontal");
        horizontal->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
        horizontal->SetAlignedPos(Vector2::Zero);
        horizontal->SetAlignedSize(Vector2::One);
        horizontal->GetProps().direction = DirectionOrientation::Horizontal;
        horizontal->GetProps().mode = DirectionalLayout::Mode::Bordered;
        AddChild(horizontal);
        
        Widget* textureBG = m_manager->Allocate<Widget>("TextureBG");
        textureBG->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
        textureBG->SetAlignedPos(Vector2::Zero);
        textureBG->SetAlignedSize(Vector2(0.7f, 1.0f));
        textureBG->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
        horizontal->AddChild(textureBG);
        
        Widget* texturePanel = m_manager->Allocate<Widget>("TexturePanel");
        texturePanel->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
        texturePanel->SetAlignedPos(Vector2::Zero);
        texturePanel->SetAlignedSize(Vector2::One);
        texturePanel->GetWidgetProps().drawBackground = true;
        texturePanel->GetWidgetProps().colorBackground = Theme::GetDef().background0;
        texturePanel->GetWidgetProps().outlineThickness = 0.0f;
        texturePanel->GetWidgetProps().rounding = 0.0f;
        textureBG->AddChild(texturePanel);
        
        Widget* texture = m_manager->Allocate<Widget>("Texture");
        texture->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
        texture->SetAlignedPos(Vector2::One * 0.5f);
        texture->SetAlignedSize(Vector2::One);
        texture->SetAnchorX(Anchor::Center);
        texture->SetAnchorY(Anchor::Center);
        texturePanel->AddChild(texture);
                
        ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
        scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
        scroll->SetAlignedPos(Vector2(0.7f, 0.0f));
        scroll->SetAlignedSize(Vector2(0.3f, 1.0f));
        scroll->GetProps().direction = DirectionOrientation::Vertical;
        horizontal->AddChild(scroll);
        
        DirectionalLayout* inspector = m_manager->Allocate<DirectionalLayout>("Inspector");
        inspector->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
        inspector->SetAlignedPos(Vector2::Zero);
        inspector->SetAlignedSize(Vector2::One);
        inspector->GetProps().direction = DirectionOrientation::Vertical;
        inspector->GetWidgetProps().childPadding = Theme::GetDef().baseIndentInner;
        inspector->GetWidgetProps().clipChildren = true;
        inspector->GetWidgetProps().childMargins.left = Theme::GetDef().baseBorderThickness;
        inspector->GetWidgetProps().childMargins.top = Theme::GetDef().baseIndent;
        scroll->AddChild(inspector);
        
        m_texturePanel = texture;
        m_inspector = inspector;
	}

    void PanelTextureViewer::Initialize()
    {
        if(m_texture != nullptr)
            return;
        
        ResourceDef def = {
            .id = m_subData,
            .tid = GetTypeID<Texture>(),
        };
        m_editor->GetResourceManagerV2().LoadResourcesFromFile(m_editor, m_editor->GetProjectManager().GetProjectData(), {def}, 0);
        m_editor->GetResourceManagerV2().WaitForAll();
        m_texture = m_editor->GetResourceManagerV2().GetResource<Texture>(def.id);
        m_texturePanel->GetWidgetProps().drawBackground = true;
        m_texturePanel->GetWidgetProps().fitTexture = true;
        m_texturePanel->GetWidgetProps().rawTexture = m_texture;
        m_texturePanel->GetWidgetProps().colorBackground = Color::White;
        m_texturePanel->GetWidgetProps().outlineThickness = 0.0f;
        m_texturePanel->GetWidgetProps().rounding = 0.0f;
        GetWidgetProps().debugName = "Texture: " + m_texture->GetName();
        
        m_textureName = m_texture->GetName();
        m_textureSize = TO_STRING(m_texture->GetSize().x) + " x " + TO_STRING(m_texture->GetSize().y);
        
        CommonWidgets::BuildClassReflection(m_inspector, this, ReflectionSystem::Get().Resolve<PanelTextureViewer>(), [](const MetaType& meta, FieldBase* field){});
        CommonWidgets::BuildClassReflection(m_inspector, &m_texture->GetMeta(), ReflectionSystem::Get().Resolve<Texture::Metadata>(), [](const MetaType& meta, FieldBase* field){
            
            
        });
    }

    void PanelTextureViewer::Destruct()
    {
        Panel::Destruct();
        m_editor->GetResourceManagerV2().UnloadResources({m_texture});
    }

    void PanelTextureViewer::SaveLayoutToStream(OStream &stream)
    {
        
    }

    void PanelTextureViewer::LoadLayoutFromStream(IStream &stream)
    {
        
    }
} // namespace Lina::Editor
