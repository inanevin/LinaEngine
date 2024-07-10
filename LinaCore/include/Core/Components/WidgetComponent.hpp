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
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/Graphics/GUI/GUIRenderer.hpp"

namespace Lina
{
	class WidgetComponent : public Component
	{
	public:
		virtual void Create() override;
		virtual void Destroy() override;
		virtual void SaveToStream(OStream& stream) const override;
		virtual void LoadFromStream(IStream& stream) override;
		virtual void PreTick() override;
		virtual void Tick(float delta) override;

		void SetWidget(StringID sid);
		void SetMaterial(StringID sid);

		virtual TypeID GetComponentType() override
		{
			return GetTypeID<WidgetComponent>();
		}

		inline GUIWidget* GetWidget() const
		{
			return m_targetWidget.raw;
		}

		inline GUIRenderer& GetGUIRenderer()
		{
			return m_guiRenderer;
		}

		inline void SetCanvasSize(const Vector2ui& sz)
		{
			m_canvasSize = sz;
		}

		inline const Vector2ui& GetCanvasSize() const
		{
			return m_canvasSize;
		}

		inline Material* GetMaterial() const
		{
			return m_material.raw;
		}

	private:
		ResRef<GUIWidget> m_targetWidget;
		ResRef<Material>  m_material;
		GUIRenderer		  m_guiRenderer;
		Vector2ui		  m_canvasSize = Vector2ui(100, 100);
	};

	LINA_REFLECTCOMPONENT_BEGIN(WidgetComponent, "WidgetComponent", "Graphics")
	LINA_REFLECTCOMPONENT_END(WidgetComponent);
} // namespace Lina
