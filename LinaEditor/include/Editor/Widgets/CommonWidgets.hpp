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
#include "Editor/Graphics/EditorGfxHelpers.hpp"

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
	struct ResourceDirectory;

	enum class FieldType;
	class FieldBase;
	class MetaType;
	class InputField;
	class ProjectData;
} // namespace Lina

namespace LinaGX
{
	class Window;
}

namespace Lina::Editor
{
	class InfoTooltip;
	class ItemController;

	class CommonWidgets
	{
	public:
		struct GenericPopupButton
		{
			String			 title = "";
			Delegate<void()> onPressed;
		};

		static DirectionalLayout* BuildWindowButtons(Widget* source);
		static InfoTooltip*		  ThrowInfoTooltip(const String& str, LogLevel level, float time, Widget* source);
		static InfoTooltip*		  ThrowInfoTooltip(const String& str, LogLevel level, float time, WidgetManager* manager, const Vector2& targetPos);

		static Widget* BuildGenericPopupWithButtons(Widget* src, const String& desc, const Vector<GenericPopupButton>& buttons);
		static Widget* BuildGenericPopupProgress(Widget* src, const String& desc, bool isRotatingCircle);

		static float GetPopupWidth(LinaGX::Window* window);

		static Widget* GetPopupItemWithSelectionToggle(Widget* source, const String& title, bool isSelected);

		static Widget* BuildPayloadForPanel(Widget* src, const String& name);

		static Widget* BuildStringLayout(Widget* src, const String& title, String* str, uint32 dependencies = 0, Delegate<void()> editStarted = nullptr, Delegate<void()> edited = nullptr, Delegate<void()> editEnd = nullptr);
		static Widget* BuildVector3FLayout(Widget* src, const String& title, float step, Vector3* vector, uint32 dependencies = 0, Delegate<void()> editStarted = nullptr, Delegate<void()> edited = nullptr, Delegate<void()> editEnd = nullptr);
		static Widget* BuildFloatLayout(Widget* src, const String& title, float step, float* val, uint32 dependencies = 0, Delegate<void()> editStarted = nullptr, Delegate<void()> edited = nullptr, Delegate<void()> editEnd = nullptr);

		static InputField* BuildFloatField(Widget* src, void* ptr, uint8 bits, bool isInt, bool isUnsigned, bool hasLimits, float minLimit = 0.0f, float maxLimit = 1.0f, float step = 0.1f, bool canSelectThemeValues = false);
		static Widget*	   BuildTextureField(Widget* src, ProjectData* project, LinaTexture2D* txt, uint32 dependencies, const String& title, bool* foldVal, Delegate<void()> onValueChanged);
		static Widget*	   BuildResourceField(Widget* src, ResourceID* currentResourceID, TypeID targetType, Delegate<void(ResourceDirectory*)> onSelected);

		static Widget*	   BuildFieldLayout(Widget* src, uint32 dependencies, const String& title, bool isFoldLayout, bool* foldVal = nullptr);
		static Widget*	   BuildFieldLayoutWithRightSide(Widget* src, uint32 dependencies, const String& title, bool isFoldLayout, bool* foldVal = nullptr, float horizontalSz = 0.5f);
		static Widget*	   BuildField(Widget* src, const String& title, void* memberVariablePtr, MetaType* metaType, FieldBase* field, FieldType fieldType, Delegate<void(MetaType* meta, FieldBase* field)> onFieldChanged, int32 vectorElementIndex = -1);
		static FoldLayout* BuildFoldTitle(Widget* src, const String& title, bool* foldValue);
		static Widget*	   BuildColorGradSlider(Widget* src, ColorGrad* color, MetaType* meta, FieldBase* field, Delegate<void(MetaType* meta, FieldBase* field)> onFieldChanged);
		static void		   BuildClassReflection(Widget* owner, void* obj, MetaType* meta, Delegate<void(MetaType* meta, FieldBase* field)> onFieldChanged);
		static void		   RefreshVector(Widget* owningFold, FieldBase* field, void* vectorPtr, MetaType* meta, FieldType subType, int32 elementIndex, Delegate<void(MetaType* meta, FieldBase* field)> onFieldChanged, bool disallowAddDelete);

		static Widget* ThrowResourceSelector(Widget* src, ResourceID currentResourceID, TypeID resourceType, Delegate<void(ResourceDirectory*)>&& onSelected);
		static Widget* BuildThumbnailTooltip(void* thumbnailOwner);

		static Widget* BuildSimpleForegroundLockText(Widget* src, const String& text);

		static Button* BuildIconButton(Widget* src, const String& icon);

		struct ResDirItemProperties
		{
			String			   chevron		   = "";
			String			   chevronAlt	   = "";
			String			   typeText		   = "";
			Color			   typeColor	   = Color::White;
			String			   mainIcon		   = "";
			Color			   mainIconColor   = Color::White;
			TextureAtlasImage* image		   = nullptr;
			String			   title		   = "";
			String			   footerIcon	   = "";
			Color			   footerIconColor = Color::White;
			float			   margin		   = 0.0f;
			bool*			   unfoldValue	   = nullptr;
			void*			   userData		   = nullptr;
		};

		static FoldLayout* BuildTreeItem(Widget* src, const ResDirItemProperties& props);

		struct EntityItemProperties
		{
			String icon			  = "";
			Color  iconColor	  = Color::White;
			String visibilityIcon = "";
			String title		  = "";
			bool   hasChildren	  = false;
			float  margin		  = 0.0f;
			bool*  unfoldValue	  = nullptr;
			void*  userData		  = nullptr;
		};

		static FoldLayout* BuildEntityItem(Widget* src, const EntityItemProperties& props);

	private:
	};
} // namespace Lina::Editor
