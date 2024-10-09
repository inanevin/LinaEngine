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

#include "Common/JobSystem/JobSystem.hpp"
#include <LinaGX/Core/Window.hpp>

namespace LinaGX
{
	class Window;
	class Instance;
} // namespace LinaGX

namespace Lina
{
	struct SystemInitializationInfo;
	class CoreResourcesRegistry;
	class ApplicationDelegate;

	class Application : public LinaGX::WindowListener
	{
	public:
		Application(){};
		~Application(){};

		bool		 Initialize(const SystemInitializationInfo& initInfo);
		void		 Tick();
		void		 Render();
		void		 Shutdown();
		virtual void OnWindowClose(LinaGX::Window* window) override;
		virtual void OnWindowSizeChanged(LinaGX::Window* window, const LinaGX::LGXVector2ui&) override;

		void			DestroyApplicationWindow(StringID sid);
		LinaGX::Window* CreateApplicationWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2ui& size, uint32 style, LinaGX::Window* parentWindow = nullptr);
		LinaGX::Window* GetApplicationWindow(StringID sid);

		inline void Quit()
		{
			m_exitRequested = true;
		}

		inline bool GetExitRequested()
		{
			return m_exitRequested;
		}

		inline void SetListener(ApplicationDelegate* listener)
		{
			m_appDelegate = listener;
		}

		inline ApplicationDelegate* GetAppDelegate()
		{
			return m_appDelegate;
		}

		static LinaGX::Instance* GetLGX()
		{
			return s_lgx;
		}

		inline JobExecutor& GetExecutor()
		{
			return m_executor;
		}

	private:
		void SetFrameCap(int64 microseconds);
		void SetFixedTimestep(int64 microseconds);
		void LoadPlugins();
		void UnloadPlugins();
		void CalculateTime();

	private:
		static LinaGX::Instance* s_lgx;
		ApplicationDelegate*	 m_appDelegate	 = nullptr;
		bool					 m_exitRequested = false;
		bool					 m_isIdleMode	 = false;
		JobExecutor				 m_executor;

		// Time
		int64 m_frameCapAccumulator		 = 0;
		int64 m_fixedTimestepAccumulator = 0;
	};
} // namespace Lina
