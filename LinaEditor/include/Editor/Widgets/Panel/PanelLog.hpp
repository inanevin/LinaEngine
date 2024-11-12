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

namespace Lina
{
	class DirectionalLayout;
	class ScrollArea;
} // namespace Lina

namespace Lina::Editor
{

	class PanelLog : public Panel, public LogListener
	{
	private:
		static constexpr uint32 LOG_LEVEL_COUNT = 4;

		struct LogLevelData
		{
			LogLevel level = LogLevel::Info;
			bool	 show  = false;
			Color	 color = {};
			String	 title = "";
		};

	public:
		PanelLog() : Panel(PanelType::Log){};
		virtual ~PanelLog() = default;

		virtual void Construct() override;
		virtual void Destruct() override;
		virtual void PreTick() override;

		virtual void SaveLayoutToStream(OStream& stream);
		virtual void LoadLayoutFromStream(IStream& stream);

	protected:
		virtual void OnLog(LogLevel level, const char* msg) override;

	private:
		void UpdateTextVisibility();

	private:
		DirectionalLayout*			m_logLayout = nullptr;
		ScrollArea*					m_logScroll = nullptr;
		String						m_searchStr = "";
		Vector<Widget*>				m_newLogs;
		static Vector<LogLevelData> s_logLevels;
	};

	LINA_WIDGET_BEGIN(PanelLog, Hidden)
	LINA_CLASS_END(PanelLog)

} // namespace Lina::Editor
