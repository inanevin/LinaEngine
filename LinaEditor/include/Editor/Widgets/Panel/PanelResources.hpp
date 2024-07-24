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

#include "Editor/Widgets/Panel/Panel.hpp"
#include "Core/GUI/Widgets/Compound/FileMenu.hpp"
#include "Editor/Widgets/Compound/SelectableListLayout.hpp"
#include "Editor/IO/FileManager.hpp"
#include "Editor/Widgets/Layout/ItemLayout.hpp"

namespace Lina
{
	class LayoutBorder;
	class Text;
	class DirectionalLayout;
	class GridLayout;
	class Selectable;
	class ScrollArea;
} // namespace Lina

namespace Lina::Editor
{
	struct DirectoryItem;
	class ItemLayout;
	class Editor;

	class PanelResources : public Panel, public FileMenuListener, public SelectableListLayoutListener, public FileManagerListener, public EditorPayloadListener
	{

	private:
		enum class ContentSorting
		{
			Alphabetical = 0,
			TypeBased,
		};

	public:
		static constexpr float MAX_CONTENTS_SIZE   = 8.0f;
		static constexpr float MIN_CONTENTS_SIZE   = 4.0f;
		static constexpr float LIST_CONTENTS_LIMIT = 4.25f;

		PanelResources() : Panel(PanelType::Resources, 0){};
		virtual ~PanelResources() = default;

		virtual void Initialize() override;
		virtual void Construct() override;
		virtual void Destruct() override;
		virtual void Draw() override;
		static void	 SaveLayoutDefaults(OStream& stream);
		void		 RequestDelete(bool isFolderBrowser);
		void		 RequestDuplicate(DirectoryItem* item, bool isFolderBrowser);
		void		 AddToFavourites(DirectoryItem* it);
		void		 RemoveFromFavourites(DirectoryItem* it);

	protected:
		virtual bool OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData) override;
		virtual void OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData) override;
		virtual void OnFileManagerThumbnailsGenerated(DirectoryItem* src, bool wasRecursive) override;
		virtual void LoadLayoutFromStream(IStream& stream) override;
		virtual void SaveLayoutToStream(OStream& stream) override;
		virtual void OnPayloadStarted(PayloadType type, Widget* payload) override;
		virtual void OnPayloadEnded(PayloadType type, Widget* payload) override;
		virtual bool OnPayloadDropped(PayloadType type, Widget* payload) override;

	private:
		Widget*		   BuildFileBrowserTop();
		Widget*		   BuildFileBrowserBottom();
		Widget*		   BuildFileBrowser();
		Widget*		   BuildFolderBrowser();
		void		   SwitchFileBrowserContents(bool showAsGrid);
		ItemDefinition CreateDefinitionForItem(DirectoryItem* it, bool onlyDirectories, bool unfoldOverride);
		void		   SelectDirectory(DirectoryItem* item);
		DirectoryItem* FindFirstParentExcluding(DirectoryItem* searchStart, const Vector<DirectoryItem*>& excludedItems);
		void		   DeleteItems(const Vector<DirectoryItem*>& items, bool isFolderLayout);
		virtual bool   OnKey(uint32 key, int32 scancode, LinaGX::InputAction act) override;

	private:
		LayoutBorder* m_border			  = nullptr;
		Editor*		  m_editor			  = nullptr;
		Text*		  m_path			  = nullptr;
		Text*		  m_itemCount		  = nullptr;
		Text*		  m_selectedItemCount = nullptr;

		float  m_contentsSize			   = MAX_CONTENTS_SIZE;
		bool   m_fileBrowserContentsAsGrid = false;
		String m_folderBrowserSearchStr	   = "";
		String m_fileBrowserSearchStr	   = "";

		ItemLayout*	   m_folderBrowserItemLayout = nullptr;
		ItemLayout*	   m_fileBrowserItemLayout	 = nullptr;
		Widget*		   m_backButton				 = nullptr;
		ContentSorting m_contentSorting			 = ContentSorting::Alphabetical;
		Vector<String> m_favouriteDirectories;
		bool		   m_favouritesDummyData = false;

		DirectoryItem* m_viewDirectory = nullptr;
		bool		   m_payloadActive = false;
	};

	LINA_REFLECTWIDGET_BEGIN(PanelResources)
	LINA_REFLECTWIDGET_END(PanelResources)

} // namespace Lina::Editor
