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

#include "Core/GUI/Widgets/Widget.hpp"

namespace Lina
{
	class InputField;
	class Button;
	class DirectionalLayout;
} // namespace Lina

namespace Lina::Editor
{
	class IconTabs;
	class WindowBar;

	class ProjectSelector : public Widget
	{
	public:
		ProjectSelector()		   = default;
		virtual ~ProjectSelector() = default;

		struct Properties
		{
			Delegate<void(const String& path)> onProjectCreated;
			Delegate<void(const String& path)> onProjectOpened;
		};

		virtual void Construct() override;
		virtual void CalculateSize(float delta) override;
		virtual void PreTick() override;
		virtual void Tick(float delta) override;
		virtual void Draw(int32 threadIndex) override;

		void SetCancellable(bool isCancellable);
		void SetTab(int32 selected);

		inline Properties& GetProps()
		{
			return m_props;
		}

	private:
		DirectionalLayout* BuildLocationSelectRow(const String& dialogTitle, bool isSave);
		DirectionalLayout* BuildButtonsRow(bool isCreate);
		DirectionalLayout* BuildContentCreateNew();
		DirectionalLayout* BuildContentOpen();

	private:
		Vector2			   m_monitorSize		   = Vector2::Zero;
		DirectionalLayout* m_content			   = nullptr;
		WindowBar*		   m_title				   = nullptr;
		Button*			   m_buttonCancel		   = nullptr;
		int32			   m_selected			   = -1;
		String			   m_projectPath		   = "";
		bool			   m_isCancellable		   = true;
		Widget*			   m_currentLocationButton = nullptr;
		Properties		   m_props				   = {};
		IconTabs*		   m_iconTabs			   = nullptr;
	};

} // namespace Lina::Editor
