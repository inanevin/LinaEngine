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

#include "Common/Data/String.hpp"
#include "Common/Math/Color.hpp"

namespace Lina
{
	class Button;
	class DirectionalLayout;
	class Widget;
	class WidgetManager;
	class Popup;
	class Text;
	class FoldLayout;
	struct TextureAtlasImage;
	class ResourceDirectory;

	enum class FieldType;
	class FieldBase;
	class MetaType;
} // namespace Lina

namespace LinaGX
{
	class Window;
}

namespace Lina::Editor
{
	class GenericPopup;
	class InfoTooltip;
	class ItemController;
	struct DirectoryItem;

	class CommonWidgets
	{
	public:
		static DirectionalLayout* BuildWindowButtons(Widget* source);
		static InfoTooltip*		  ThrowInfoTooltip(const String& str, LogLevel level, float time, Widget* source);
		static InfoTooltip*		  ThrowInfoTooltip(const String& str, LogLevel level, float time, WidgetManager* manager, const Vector2& targetPos);

		static GenericPopup* ThrowGenericPopup(const String& title, const String& text, const String& icon, Widget* source);

		static float GetPopupWidth(LinaGX::Window* window);

		static Widget* GetPopupItemWithSelectionToggle(Widget* source, const String& title, bool isSelected);

		static Widget* BuildPayloadForPanel(Widget* src, const String& name);
		static Widget* BuildPayloadForDirectoryItem(Widget* src, DirectoryItem* item);
		static Widget* BuildDefaultFoldItem(Widget* src, void* userdata, float margin, const String& icon, const Color& iconColor, const String& title, bool hasChildren, bool* unfoldVal, bool isRoot = false, bool boldText = false);
		static Widget* BuildDefaultListItem(Widget* src, void* userData, float margin, const String& icon, const Color& iconColor, const String& title, bool foldNudge);
		static Widget* BuildTexturedListItem(Widget* src, void* userData, float margin, TextureAtlasImage* txt, const String& title);
		static Widget* BuildDirectoryItemFolderView(Widget* src, DirectoryItem* item, float margin);
		static Widget* BuildDirectoryItemListView(Widget* src, DirectoryItem* item);
		static Widget* BuildDirectoryItemGridView(Widget* src, DirectoryItem* item, const Vector2& itemSize);

		static Widget*	   BuildFieldLayout(Widget* src, uint32 dependencies, const String& title, bool isFoldLayout, bool* foldVal = nullptr);
		static Widget*	   BuildField(Widget* src, const String& title, void* memberVariablePtr, MetaType& metaType, FieldBase* field, FieldType fieldType, Delegate<void(const MetaType& meta, FieldBase* field)> onFieldChanged, int32 vectorElementIndex = -1);
		static FoldLayout* BuildFoldTitle(Widget* src, const String& title, bool* foldValue);
		static Widget*	   BuildColorGradSlider(Widget* src, ColorGrad* color, MetaType& meta, FieldBase* field, Delegate<void(const MetaType& meta, FieldBase* field)> onFieldChanged);
		static void		   BuildClassReflection(Widget* owner, void* obj, MetaType& meta, Delegate<void(const MetaType& meta, FieldBase* field)> onFieldChanged);
		static void		   RefreshVector(Widget* owningFold, FieldBase* field, void* vectorPtr, MetaType* meta, FieldType subType, int32 elementIndex, Delegate<void(const MetaType& meta, FieldBase* field)> onFieldChanged, bool disallowAddDelete);

		static Widget* ThrowResourceSelector(Widget* src, TypeID reosurceType, Delegate<void(ResourceDirectory*)>&& onSelected);
		static Widget* BuildThumbnailTooltip(void* thumbnailOwner);

	private:
	};
} // namespace Lina::Editor
