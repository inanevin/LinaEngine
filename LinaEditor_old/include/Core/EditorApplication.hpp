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

#ifndef EditorApplication_HPP
#define EditorApplication_HPP

#include "Core/Application.hpp"
#include "Editor.hpp"

namespace Lina::Editor
{
	class EditorApplication : public Lina::Application, public ISystemEventListener
	{
	public:
		EditorApplication() : m_editor(&m_engine){};
		virtual ~EditorApplication() = default;

		virtual void PreTick() override;
		virtual void Tick() override;
		virtual void Shutdown() override;
		virtual void OnSystemEvent(SystemEvent eventType, const Event& ev);

		virtual Bitmask32 GetSystemEventMask()
		{
			return m_systemEventMask;
		}

	protected:
		virtual void SetupEnvironment() override;
		virtual void CreateMainWindow(const SystemInitializationInfo& initInfo) override;
		virtual void OnInited() override;

	private:
		uint32 m_systemEventMask	   = EVS_ResourceLoadTaskCompleted;
		uint32 m_loadCoreResourcesTask = 0;
		bool   m_doneWithSplash		   = false;
		Editor m_editor;
	};
} // namespace Lina::Editor

#endif
