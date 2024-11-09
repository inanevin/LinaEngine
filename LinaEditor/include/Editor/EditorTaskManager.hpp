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
#include "Common/Data/Map.hpp"
#include "Common/Data/Functional.hpp"
#include "Common/JobSystem/JobSystem.hpp"

namespace Lina
{
	class Text;
}

namespace LinaGX
{
	class Window;
}

namespace Lina::Editor
{
	class Editor;
	class ProgressCircleFill;

	struct EditorTask
	{
		Delegate<void()> task;
		Delegate<void()> onComplete;
		Atomic<bool>	 isComplete;
		bool			 useProgress  = false;
		float			 progress	  = 0.0f;
		String			 title		  = "";
		LinaGX::Window*	 ownerWindow  = nullptr;
		String			 progressText = "";
		void*			 userData	  = nullptr;
	};

	class EditorTaskManager
	{
	public:
		void Initialize(Editor* editor);
		void Shutdown();
		void PreTick();

		EditorTask* CreateTask();
		void		AddTask(EditorTask* task);

	private:
		void StartTask(EditorTask* task);

	private:
		JobExecutor			m_executor;
		Vector<EditorTask*> m_waitingTasks;
		Text*				m_lockProgress = nullptr;
		ProgressCircleFill* m_progressFill = nullptr;
		Editor*				m_editor	   = nullptr;
		EditorTask*			m_runningTask  = nullptr;
	};

} // namespace Lina::Editor
