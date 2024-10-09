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
#include "Core/World/EntityWorld.hpp"

namespace Lina
{
	void WidgetComponent::SaveToStream(OStream& stream) const
	{
		stream << m_targetWidget;
	}

	void WidgetComponent::LoadFromStream(IStream& stream)
	{
		stream >> m_targetWidget;
	}

	void WidgetComponent::SetMaterial(ResourceID id)
	{
		m_material.id = id;
		// m_material.raw = m_resourceManager->GetResource<Material>(id);
	}

	void WidgetComponent::SetWidget(ResourceID id)
	{
		// m_targetWidget.sid = sid;
		// m_targetWidget.raw = m_resourceManager->GetResource<GUIWidget>(sid);
		// auto* root		   = m_guiRenderer.GetGUIRoot();
		// root->DeallocAllChildren();
		// root->RemoveAllChildren();
		//
		// IStream stream;
		// m_targetWidget.raw->CopyRootBlob(stream);
		// root->LoadFromStream(stream);
		// stream.Destroy();
	}
} // namespace Lina
