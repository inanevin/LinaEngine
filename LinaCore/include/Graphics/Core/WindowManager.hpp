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

#ifndef WindowManager_HPP
#define WindowManager_HPP

#include "System/ISubsystem.hpp"
#include "Data/HashMap.hpp"
#include "Core/Common.hpp"

namespace Lina
{
	struct MonitorInfo
	{
		bool	 isPrimary	  = false;
		Vector2i size		  = Vector2i::Zero;
		Vector2i workArea	  = Vector2i::Zero;
		float	 contentScale = 1.0f;
	};

	class IWindow;
	class GfxManager;

	class WindowManager final : public ISubsystem
	{
	public:
		WindowManager(ISystem* sys) : ISubsystem(sys, SubsystemType::WindowManager){};
		~WindowManager() = default;

		virtual void PreInitialize(const SystemInitializationInfo& inf) override;
		virtual void Initialize(const SystemInitializationInfo& initInfo){};
		virtual void Shutdown() override;

		void		CreateAppWindow(StringID sid, WindowStyle style, const char* title, const Vector2i& pos, const Vector2i& size);
		void		DestroyAppWindow(StringID sid);
		IWindow*	GetWindow(StringID sid);
		void		OnWindowFocused(StringID sid);
		int			GetWindowZOrder(StringID sid);
		void		SetVsync(VsyncMode mode);
		MonitorInfo GetMonitorInfoFromWindow(IWindow* window) const;

		inline const Vector<MonitorInfo>& GetMonitors() const
		{
			return m_monitors;
		}

		inline MonitorInfo* GetPrimaryMonitor()
		{
			for (auto& m : m_monitors)
			{
				if (m.isPrimary)
					return &m;
			}

			return nullptr;
		}

	private:
		VsyncMode					m_vsync		 = VsyncMode::None;
		GfxManager*					m_gfxManager = nullptr;
		HashMap<StringID, IWindow*> m_windows;
		Vector<StringID>			m_drawOrders;
		Vector<MonitorInfo>			m_monitors;
	};
} // namespace Lina

#endif
