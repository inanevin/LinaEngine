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
#include "Core/GUI/Widgets/Compound/FileMenu.hpp"
#include "Common/Data/Vector.hpp"
#include "Core/Resources/ResourceDirectory.hpp"
#include "Editor/Project/ProjectManager.hpp"

namespace Lina
{
	class DirectionalLayout;
	struct ResourceDirectory;
} // namespace Lina

namespace Lina::Editor
{

	class Editor;
	class ItemController;

	class ResourceDirectoryBrowser : public Widget, public FileMenuListener, public ProjectManagerListener
	{
	public:
		enum class Filter : uint8
		{
			None = 0,
			Favourites,
			Max,
		};

		ResourceDirectoryBrowser()			= default;
		virtual ~ResourceDirectoryBrowser() = default;

		struct Properties
		{
			TypeID itemTypeIDFilter = 0;
		};

		virtual void  Construct() override;
		virtual void  Destruct() override;
		virtual void  Initialize() override;
		void		  RefreshDirectory();
		void		  SetFilter(Filter filter);
		void		  SetSearchStr(const String& str);
		static String GetFilterStr(Filter filter);

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline ItemController* GetItemController() const
		{
			return m_controller;
		}

	protected:
		virtual bool OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData) override;
		virtual void OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData) override;
		virtual void OnProjectOpened(ProjectData* data) override
		{
			RefreshDirectory();
		}
		virtual void OnProjectResourcesRefreshed() override
		{
			RefreshDirectory();
		}

	private:
		void AddItem(Widget* parent, ResourceDirectory* item, float margin);
		void AddItemForDirectory(Widget* parent, ResourceDirectory* dir, float margin);
		void RequestRename(ResourceDirectory* dir);
		void RequestDelete(Vector<ResourceDirectory*> dirs);
		void RequestDuplicate(Vector<ResourceDirectory*> dirs);
		void DeleteItems(Vector<ResourceDirectory*> dirs);
		void DropPayload(ResourceDirectory* target);
		bool CheckIfContainsSearchStr(ResourceDirectory* dir);
		bool CheckIfContainsEngineResource(const Vector<ResourceDirectory*>& dirs);
		bool IsChildInFavs(ResourceDirectory* dir) const;

	private:
		Properties											 m_props;
		ItemController*										 m_controller		  = nullptr;
		DirectionalLayout*									 m_layout			  = nullptr;
		Editor*												 m_editor			  = nullptr;
		Vector<ResourceDirectory*>							 m_payloadItems		  = {};
		Vector<Pair<ResourceDirectory*, TextureAtlasImage*>> m_importingResources = {};
		ResourceDirectory									 m_linaAssets;
		Filter												 m_filter	 = Filter::None;
		String												 m_searchStr = "";
	};

	LINA_WIDGET_BEGIN(ResourceDirectoryBrowser, Editor)
	LINA_CLASS_END(ResourceDirectoryBrowser)

} // namespace Lina::Editor
