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

#ifndef LGXWrapper_HPP
#define LGXWrapper_HPP

#include "System/Subsystem.hpp"
#include "Math/Vector.hpp"
#include "Core/StringID.hpp"

namespace LinaGX
{
	class Instance;
	class WindowManager;
	class Input;
	class Window;
} // namespace LinaGX

namespace Lina
{
	class GfxManager;

	class LGXWrapper : public Subsystem
	{
	public:
		LGXWrapper(System* sys);
		virtual ~LGXWrapper() = default;

		void				   PreInitialize(const SystemInitializationInfo& initInfo);
		virtual void		   Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void		   PreShutdown() override;
		virtual void		   Shutdown() override;
		void				   DestroyApplicationWindow(StringID sid);
		LinaGX::Window*		   CreateApplicationWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2ui& size, uint32 style, LinaGX::Window* parentWindow = nullptr);
		LinaGX::WindowManager* GetWindowManager();
		LinaGX::Input*		   GetInput();

		LinaGX::Instance* GetLGX()
		{
			return m_lgx;
		}

	private:
		LinaGX::Instance* m_lgx		   = nullptr;
		GfxManager*		  m_gfxManager = nullptr;
	};
} // namespace Lina

#endif
