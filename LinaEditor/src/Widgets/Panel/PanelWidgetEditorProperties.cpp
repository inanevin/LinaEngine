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

#include "Editor/Widgets/Panel/PanelWidgetEditorProperties.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"

namespace Lina::Editor
{

	void PanelWidgetEditorProperties::Construct()
	{
		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>();
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		layout->GetProps().direction		  = DirectionOrientation::Vertical;
		layout->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		layout->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		AddChild(layout);
		m_layout = layout;
	}

	void PanelWidgetEditorProperties::Refresh(Widget* w)
	{
		m_layout->DeallocAllChildren();
		m_layout->RemoveAllChildren();

		if (w == nullptr)
			return;

		MetaType&		   type	  = ReflectionSystem::Get().Resolve<Widget>();
		Vector<FieldBase*> fields = type.GetFieldsOrdered();

		for (FieldBase* field : fields)
		{
			const String   title	 = field->GetProperty<String>("Title"_hs);
			const StringID type		 = field->GetProperty<StringID>("Type"_hs);
			const String   tooltip	 = field->GetProperty<String>("Tooltip"_hs);
			const StringID dependsOn = field->GetProperty<StringID>("DependsOn"_hs);
			const TypeID   subType	 = field->GetProperty<TypeID>("SubType"_hs);
			FieldValue	   val		 = field->Value(w);

			Widget* fieldWidget = CommonWidgets::BuildField(this, title, type, val, field);

			if (title.compare("Anchor X") == 0 || title.compare("Anchor Y") == 0)
			{
				Anchor*	  anchor = val.CastPtr<Anchor>();
				Dropdown* dd	 = fieldWidget->GetWidgetOfType<Dropdown>(fieldWidget);

				const Vector<String> texts	   = {"Start", "Center", "End"};
				dd->GetText()->GetProps().text = texts[static_cast<int32>(*anchor)];
				dd->GetText()->CalculateTextSize();

				dd->GetProps().onAddItems = [anchor, texts](Vector<String>& outItems, Vector<int32>& outSelected) {
					outSelected.push_back(static_cast<int32>(*anchor));
					outItems = texts;
				};
			}
			else if (title.compare("Flags") == 0)
			{
				Bitmask32* flags = val.CastPtr<Bitmask32>();

				Dropdown* dd					  = fieldWidget->GetWidgetOfType<Dropdown>(fieldWidget);
				dd->GetProps().closeOnSelect	  = false;
				dd->GetProps().switchTextOnSelect = false;

				const Vector<Pair<String, uint32>> mask = {
					{"Pos Align X", WF_POS_ALIGN_X},
					{"Pos Align Y", WF_POS_ALIGN_Y},
					{"Size Align X", WF_SIZE_ALIGN_X},
					{"Size Align Y", WF_SIZE_ALIGN_Y},
					{"Fixed Size X", WF_USE_FIXED_SIZE_X},
					{"Fixed Size Y", WF_USE_FIXED_SIZE_Y},
					{"Size X Copy Y", WF_SIZE_X_COPY_Y},
					{"Size Y Copy X", WF_SIZE_Y_COPY_X},
					{"Size X Total", WF_SIZE_X_TOTAL_CHILDREN},
					{"Size Y Total", WF_SIZE_Y_TOTAL_CHILDREN},
					{"Size X Max", WF_SIZE_X_MAX_CHILDREN},
					{"Size Y Max", WF_SIZE_Y_MAX_CHILDREN},
					{"Foreground Blocker", WF_FOREGROUND_BLOCKER},
					{"Tick After Children", WF_TICK_AFTER_CHILDREN},
					{"Size After Children", WF_SIZE_AFTER_CHILDREN},
					{"Mouse Passthrough", WF_MOUSE_PASSTHRU},
					{"Key Passthrough", WF_KEY_PASSTHRU},
					{"Owns Size", WF_OWNS_SIZE},
					{"Controls Draw Order", WF_CONTROLS_DRAW_ORDER},
					{"Controllable", WF_CONTROLLABLE},
					{"Skip Flooring", WF_SKIP_FLOORING},
				};

				dd->GetProps().onSelected = [flags, mask, dd](int32 item) -> bool {
					if (flags->IsSet(mask.at(item).second))
					{
						flags->Remove(mask.at(item).second);
						dd->GetText()->GetProps().text = TO_STRING(flags->GetValue());
						dd->GetText()->CalculateTextSize();
						return false;
					}
					else
					{
						flags->Set(mask.at(item).second);
						dd->GetText()->GetProps().text = TO_STRING(flags->GetValue());
						dd->GetText()->CalculateTextSize();
						return true;
					}
				};

				dd->GetProps().onAddItems = [flags, mask](Vector<String>& outItems, Vector<int32>& outSelectedItems) {
					int32 i = 0;
					for (const Pair<String, uint32>& p : mask)
					{
						outItems.push_back(p.first);
						if (flags->IsSet(p.second))
							outSelectedItems.push_back(i);
						i++;
					}
				};
			}
			m_layout->AddChild(fieldWidget);
		}
	}

} // namespace Lina::Editor
