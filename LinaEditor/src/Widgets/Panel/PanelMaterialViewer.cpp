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

#include "Editor/Widgets/Panel/PanelMaterialViewer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/World/WorldDisplayer.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Application.hpp"
#include "Core/Components/FlyCameraMovement.hpp"
#include "Core/Components/CameraComponent.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"

namespace Lina::Editor
{

	void PanelMaterialViewer::Construct()
	{
		PanelResourceViewer::Construct();

		WorldDisplayer* displayer = m_manager->Allocate<WorldDisplayer>("WorldDisplayer");
		displayer->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		displayer->SetAlignedPos(Vector2::Zero);
		displayer->SetAlignedSize(Vector2::One);
		m_resourceBG->AddChild(displayer);
		m_worldDisplayer = displayer;

		Widget* loading = CommonWidgets::BuildGenericPopupProgress(displayer, Locale::GetStr(LocaleStr::LoadingWorld), true);
		displayer->AddChild(loading);
	}

	void PanelMaterialViewer::Initialize()
	{
		PanelResourceViewer::Initialize();
		if (!m_resource)
			return;

		UpdateMaterialProps();

		m_world			= new EntityWorld(0, "");
		m_worldRenderer = new WorldRenderer(m_world, Vector2ui(4, 4));
		m_worldDisplayer->DisplayWorld(m_worldRenderer);
		m_editor->GetEditorRenderer().AddWorldRenderer(m_worldRenderer);

		// Setup camera.
		Entity*			   cameraEntity = m_world->CreateEntity("Camera");
		FlyCameraMovement* flyCamera	= m_world->AddComponent<FlyCameraMovement>(cameraEntity);
		flyCamera->GetFlags().Set(CF_RECEIVE_EDITOR_TICK);
		CameraComponent* cameraComp = m_world->AddComponent<CameraComponent>(cameraEntity);
		m_world->SetActiveCamera(cameraComp);
	}

	void PanelMaterialViewer::Destruct()
	{
		PanelResourceViewer::Destruct();

		if (m_world)
		{
			m_editor->GetEditorRenderer().RemoveWorldRenderer(m_worldRenderer);
			delete m_worldRenderer;
			delete m_world;
			m_worldRenderer = nullptr;
			m_world			= nullptr;
		}
	}

	void PanelMaterialViewer::OnGeneralMetaChanged(const MetaType& meta, FieldBase* field)
	{
	}

	void PanelMaterialViewer::OnResourceMetaChanged(const MetaType& meta, FieldBase* field)
	{
		// Application::GetLGX()->Join();
		// RegenGPU();
	}

	void PanelMaterialViewer::RegenHW()
	{
		Material* mat = static_cast<Material*>(m_resource);
	}

	void PanelMaterialViewer::UpdateMaterialProps()
	{
		m_materialName = m_resource->GetName();
	}

} // namespace Lina::Editor
