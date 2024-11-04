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

#include "Editor/EditorTaskManager.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Common/Math/Math.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"

namespace Lina::Editor
{
	void EditorTaskManager::Poll()
	{
		if (m_runningTask != nullptr)
		{
			if (m_runningTask->isComplete.load())
			{
				m_runningTask->onComplete();
				delete m_runningTask;
				m_runningTask = nullptr;
				m_editor->GetWindowPanelManager().UnlockAllForegrounds();
			}
			else
			{
				m_lockProgress->UpdateTextAndCalcSize(m_runningTask->progressText);
			}
		}

		if (m_runningTask == nullptr && !m_waitingTasks.empty())
		{
		}
	}

	EditorTask* Lina::Editor::EditorTaskManager::CreateTask()
	{
		LINA_ASSERT(SystemInfo::IsMainThread(), "");
		EditorTask* task = new EditorTask();
		return task;
	}

	void EditorTaskManager::AddTask(EditorTask* task, LinaGX::Window* taskOwnerWindow)
	{
		LINA_ASSERT(SystemInfo::IsMainThread(), "");
		if (m_runningTask == nullptr)
		{
			Widget* lock = m_editor->GetWindowPanelManager().LockAllForegrounds(taskOwnerWindow, Locale::GetStr(LocaleStr::WorkInProgressInAnotherWindow));
			Widget* pp	 = CommonWidgets::BuildGenericPopupProgress(lock, task->title, true);
			lock->AddChild(pp);
			m_lockProgress = static_cast<Text*>(pp->FindChildWithDebugName("Progress"));
		}

		if (m_runningTask == nullptr)
		{
			m_runningTask = task;
			Taskflow tf;
			tf.emplace(task->task);
			m_executor.RunMove(tf, [task]() { task->isComplete.store(true); });
		}
		else
		{
			m_waitingTasks.push_back(task);
		}
	}
} // namespace Lina::Editor
