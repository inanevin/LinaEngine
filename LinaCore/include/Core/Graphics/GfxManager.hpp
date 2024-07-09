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
*/

#pragma once

#include "Common/System/Subsystem.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

namespace LinaGX
{
	class Instance;
	class Window;
} // namespace LinaGX

namespace Lina
{
	class ApplicationDelegate;

	class GfxManager : public Subsystem, public LinaGX::WindowListener
	{
	private:
	public:
		GfxManager(System* sys);
		~GfxManager() = default;

		virtual void PreInitialize(const SystemInitializationInfo& initInfo) override;
		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void PreShutdown() override;
		virtual void Shutdown() override;
		virtual void PreTick() override;
		virtual void OnWindowSizeChanged(LinaGX::Window* window, const LinaGX::LGXVector2ui&) override;

		// Loop
		void WaitForSwapchains();
		void Join();
		void Poll();
		void Tick(float delta);
		void Render(StringID pool = 0);

		// Window
		void			DestroyApplicationWindow(StringID sid);
		LinaGX::Window* CreateApplicationWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2ui& size, uint32 style, LinaGX::Window* parentWindow = nullptr);
		LinaGX::Window* GetApplicationWindow(StringID sid);

		inline static LinaGX::Instance* GetLGX()
		{
			return s_lgx;
		}

	private:
		LinaGX::Instance*		 m_lgx		   = nullptr;
		ApplicationDelegate*	 m_appDelegate = nullptr;
		static LinaGX::Instance* s_lgx;
	};
} // namespace Lina
