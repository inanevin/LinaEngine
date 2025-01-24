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

#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/Data/String.hpp"

namespace Lina
{
	class Popup;
	class Button;
	class FileMenu;
	class Text;
	class Icon;
	class FileMenuPopup;

	class FileMenuItem : public Widget
	{

	public:
		FileMenuItem()			= default;
		virtual ~FileMenuItem() = default;

		struct Data
		{
			String text			   = "";
			String altText		   = "";
			String dropdownIcon	   = "";
			String headerIcon	   = "";
			Color  headerIconColor = Theme::GetDef().foreground0;
			bool   hasDropdown	   = false;
			bool   isDivider	   = false;
			bool   isDisabled	   = false;
			void*  userData		   = nullptr;
		};

		virtual void Initialize() override;
		virtual void PreTick() override;

		virtual bool OnMouse(uint32 action, LinaGX::InputAction act) override;

		float GetMaxSizeX();

		inline Icon* GetHeaderIcon() const
		{
			return m_headerIcon;
		}

		inline Text* GetText() const
		{
			return m_text;
		}

		inline Text* GetAltText() const
		{
			return m_altText;
		}

		Data& GetItemData()
		{
			return m_itemData;
		}

	private:
		friend class FileMenu;

		DirectionalLayout* m_layout = nullptr;

		FileMenu*		   m_ownerMenu	   = nullptr;
		Data			   m_itemData	   = {};
		DirectionalLayout* m_subPopup	   = nullptr;
		Text*			   m_text		   = nullptr;
		Text*			   m_altText	   = nullptr;
		Icon*			   m_headerIcon	   = nullptr;
		StringID		   m_sid		   = 0;
		FileMenuPopup*	   m_dropdownPopup = nullptr;
		void*			   m_ud			   = nullptr;

		bool m_showDD = false;
	};

	class FileMenuListener
	{
	public:
		virtual bool OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData)
		{
			return false;
		};
		virtual void OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData){};
	};

	class FileMenu : public DirectionalLayout
	{
	public:
		struct FileMenuProperties
		{
			Vector<String> buttons;
		};

		virtual void			   Construct() override;
		virtual void			   Initialize() override;
		virtual void			   PreTick() override;
		FileMenuPopup*			   CreatePopup(const Vector2& pos, const Vector<FileMenuItem::Data>& subItemData);
		void					   CreateItems(StringID sid, const Vector2& position, void* userData = nullptr);
		void					   ClearSubpopups();
		void					   KillSub(Widget* w);
		inline FileMenuProperties& GetFileMenuProps()
		{
			return m_fileMenuProps;
		}

		inline void SetListener(FileMenuListener* list)
		{
			m_listener = list;
		}

		inline FileMenuListener* GetListener() const
		{
			return m_listener;
		}

	private:
		Button*			   m_subPopupOwner = nullptr;
		FileMenuProperties m_fileMenuProps = {};
		FileMenuListener*  m_listener	   = nullptr;
		Vector<Button*>	   m_buttons	   = {};
		DirectionalLayout* m_subPopup	   = nullptr;
		Vector<Widget*>	   m_subPopups;
	};

	class FileMenuPopup : public DirectionalLayout
	{
	public:
		virtual void Construct() override;

		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;

	private:
		friend class FileMenu;
		FileMenu* m_ownerMenu = nullptr;
	};

	LINA_WIDGET_BEGIN(FileMenuPopup, Hidden)
	LINA_CLASS_END(FileMenuPopup)

	LINA_WIDGET_BEGIN(FileMenu, Hidden)
	LINA_CLASS_END(FileMenu)

	LINA_WIDGET_BEGIN(FileMenuItem, Hidden)
	LINA_CLASS_END(FileMenuItem)

} // namespace Lina
