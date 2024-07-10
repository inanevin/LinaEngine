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
#include "Common/Tween/Tween.hpp"

namespace Lina
{
	class DirectionalLayout;
	class Slider;
} // namespace Lina

namespace Lina::Editor
{
	enum class NotificationIcon
	{
		None,
		Loading,
		Info,
		Warning,
		Err,
	};

	struct NotificationDesc
	{
		NotificationIcon				   icon				  = NotificationIcon::None;
		String							   title			  = "";
		int32							   autoDestroySeconds = -1;
		String							   buttonText		  = "";
		bool							   showButton		  = false;
		Delegate<void()>				   onClicked;
		Delegate<void(float& outProgress)> onProgress;
	};

	struct NotificationItem
	{
		Tween							   tween  = {};
		DirectionalLayout*				   layout = nullptr;
		Delegate<void(float& outProgress)> onProgress;
		Delegate<void()>				   onClicked;
		Slider*							   slider			  = nullptr;
		int32							   autoDestroySeconds = -1;
		float							   timer			  = 0.0f;
		bool							   done				  = false;
	};

	class NotificationDisplayer : public Widget
	{
	public:
		NotificationDisplayer()			 = default;
		virtual ~NotificationDisplayer() = default;

		virtual void Construct() override;
		virtual void Destruct() override;
		virtual void Tick(float delta) override;
		virtual void Draw() override;

		void AddNotification(const NotificationDesc& desc);

	private:
		static constexpr float POS_SPEED	= 15.0f;
		static constexpr float SLIDER_SPEED = 8.0f;
		static constexpr float TWEEN_TIME	= 0.2f;

		Vector<NotificationItem*> m_items;
		Vector<NotificationDesc>  m_notifications;
	};

	LINA_REFLECTWIDGET_BEGIN(NotificationDisplayer)
	LINA_REFLECTWIDGET_END(NotificationDisplayer)

} // namespace Lina::Editor
