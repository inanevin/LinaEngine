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

#include "Editor/Widgets/Panel/PanelSamplerViewer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Widgets/World/WorldDisplayer.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Actions/EditorActionResources.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Application.hpp"
#include "Core/Meta/ProjectData.hpp"

namespace Lina::Editor
{

	void PanelSamplerViewer::Construct()
	{
		PanelResourceViewer::Construct();
		Widget* texture = m_manager->Allocate<Widget>("Texture");
		texture->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		texture->SetAlignedPos(Vector2::One * 0.5f);
		texture->SetAlignedSize(Vector2::One);
		texture->SetAnchorX(Anchor::Center);
		texture->SetAnchorY(Anchor::Center);

		texture->GetWidgetProps().drawBackground   = true;
		texture->GetWidgetProps().fitTexture	   = true;
		texture->GetWidgetProps().rawTexture	   = m_editor->GetApp()->GetResourceManager().GetResource<Texture>(EDITOR_TEXTURE_PROTOTYPE_DARK_ID);
		texture->GetWidgetProps().colorBackground  = Color::White;
		texture->GetWidgetProps().outlineThickness = 0.0f;
		texture->GetWidgetProps().rounding		   = 0.0f;
		texture->GetWidgetProps().lvgUserData	   = &m_guiUserData;

		m_resourceBG->AddChild(texture);
	}

	void PanelSamplerViewer::Initialize()
	{
		PanelResourceViewer::Initialize();

		if (!m_resource)
			return;

		m_guiUserData.sampler = m_resource->GetID();

		StoreEditorActionBuffer();
		UpdateResourceProperties();
		RebuildContents();
	}

	void PanelSamplerViewer::StoreEditorActionBuffer()
	{
		TextureSampler* sampler = static_cast<TextureSampler*>(m_resource);
		m_storedDesc			= sampler->GetDesc();
	}

	void PanelSamplerViewer::UpdateResourceProperties()
	{
		TextureSampler* sampler = static_cast<TextureSampler*>(m_resource);
		m_samplerName			= sampler->GetName();
	}

	void PanelSamplerViewer::RebuildContents()
	{
		TextureSampler* sampler = static_cast<TextureSampler*>(m_resource);
		m_inspector->DeallocAllChildren();
		m_inspector->RemoveAllChildren();

		CommonWidgets::BuildClassReflection(m_inspector, this, ReflectionSystem::Get().Resolve<PanelSamplerViewer>(), [](const MetaType& meta, FieldBase* field) {

		});

		CommonWidgets::BuildClassReflection(m_inspector, &sampler->GetDesc(), ReflectionSystem::Get().Resolve<LinaGX::SamplerDesc>(), [this, sampler](const MetaType& meta, FieldBase* field) {
			EditorActionResourceSampler::Create(m_editor, sampler->GetID(), m_storedDesc, sampler->GetDesc());
		});

		if (m_previewOnly)
			DisableRecursively(m_inspector);
	}

} // namespace Lina::Editor
