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

#include "Core/Components/WidgetComponent.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/ShapeRect.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Common/Serialization/VectorSerialization.hpp"
#include "Common/System/System.hpp"

namespace Lina
{
	void WidgetComponent::Create()
	{
		auto* gfxMan = m_world->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		m_guiRenderer.Create(gfxMan, gfxMan->GetApplicationWindow(LINA_MAIN_SWAPCHAIN));
	}

	void WidgetComponent::Destroy()
	{
		m_guiRenderer.Destroy();
	}

	void WidgetComponent::SaveToStream(OStream& stream) const
	{
		m_targetWidget.SaveToStream(stream);
	}

	void WidgetComponent::LoadFromStream(IStream& stream)
	{
		m_targetWidget.LoadFromStream(stream);
	}

	void WidgetComponent::PreTick()
	{
		m_guiRenderer.PreTick();
	}

	void WidgetComponent::Tick(float delta)
	{
		m_guiRenderer.Tick(delta, m_canvasSize);
	}

	void WidgetComponent::SetWidget(StringID sid)
	{
		m_targetWidget.sid = sid;
		m_targetWidget.raw = m_resourceManager->GetResource<GUIWidget>(sid);
		auto* root		   = m_guiRenderer.GetGUIRoot();
		root->DeallocAllChildren();
		root->RemoveAllChildren();

		IStream stream;
		m_targetWidget.raw->CopyRootBlob(stream);
		root->LoadFromStream(stream);
		stream.Destroy();
	}
} // namespace Lina
