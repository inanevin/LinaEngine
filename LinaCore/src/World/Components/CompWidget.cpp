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

#include "Core/World/Components/CompWidget.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina
{
	CompWidget::~CompWidget()
	{
		if (m_widgetManager.GetRoot())
			m_widgetManager.Shutdown();
	}

	void CompWidget::CollectReferences(HashSet<ResourceID>& refs)
	{
		refs.insert(m_widget);
	}

	void CompWidget::StoreReferences()
	{
		m_widgetPtr = m_resourceManager->GetIfExists<GUIWidget>(m_widget);
		if (!m_widgetPtr)
			return;

		m_widgetManager.Initialize(m_resourceManager, m_world->GetScreen().GetOwnerWindow(), &m_world->GetLVGDrawer());

		m_widgetManager.GetRoot()->DeallocAllChildren();
		m_widgetManager.GetRoot()->RemoveAllChildren();

		Widget*			 root	= m_widgetManager.GetRoot();
		const RawStream& stream = m_widgetPtr->GetStream();
		IStream			 istream;
		istream.Create(stream.GetRaw(), stream.GetSize());
		root->LoadFromStream(istream);
		istream.Destroy();
	}

} // namespace Lina
