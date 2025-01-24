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

#include "Core/World/Component.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"

namespace Lina
{

	class CompWidget : public Component
	{
	public:
		CompWidget() : Component(GetTypeID<CompWidget>(), 0){};
		virtual ~CompWidget();

		virtual void CollectReferences(HashSet<ResourceID>& refs) override;
		virtual void StoreReferences() override;

		virtual void SaveToStream(OStream& stream) const
		{
			stream << m_widget << m_is3D << m_size3D;
		}

		virtual void LoadFromStream(IStream& stream)
		{
			stream >> m_widget >> m_is3D >> m_size3D;
		}

		inline void SetWidget(ResourceID id)
		{
			m_widget = id;
		}

		inline ResourceID GetWidget() const
		{
			return m_widget;
		}

		inline GUIWidget* GetWidgetPtr() const
		{
			return m_widgetPtr;
		}

		inline WidgetManager& GetWidgetManager()
		{
			return m_widgetManager;
		}

		inline bool GetIs3D() const
		{
			return m_is3D;
		}

		inline const Vector2& GetSize3D() const
		{
			return m_size3D;
		}

	private:
		LINA_REFLECTION_ACCESS(CompWidget);
		ResourceID m_widget = 0;

		bool		  m_is3D		  = true;
		Vector2		  m_size3D		  = Vector2(10, 10);
		GUIWidget*	  m_widgetPtr	  = nullptr;
		WidgetManager m_widgetManager = {};
	};

	LINA_COMPONENT_BEGIN(CompWidget, "Graphics")
	LINA_FIELD(CompWidget, m_widget, "Widget", FieldType::ResourceID, GetTypeID<GUIWidget>())
	LINA_FIELD(CompWidget, m_is3D, "Is 3D", FieldType::Boolean, 0)
	LINA_FIELD(CompWidget, m_size3D, "3D Size", FieldType::Vector2, 0)
	LINA_FIELD_DEPENDENCY_OP(CompWidget, m_size3D, "m_is3D", "1", "eq")
	LINA_CLASS_END(CompWidget)

} // namespace Lina
