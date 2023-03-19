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

#ifndef Application_HPP
#define Application_HPP

#include "Core/Engine.hpp"
#include "Data/Vector.hpp"
#include "Data/HashMap.hpp"

namespace Lina
{
	struct SystemInitializationInfo;
	class CoreResourcesRegistry;

	class Application
	{
	public:
		Application() : m_engine(this){};
		virtual ~Application(){};

		virtual void Initialize(const SystemInitializationInfo& initInfo);
		virtual void PostInitialize(const SystemInitializationInfo& initInfo);
		virtual void Tick();
		virtual void Shutdown();

		inline void Quit()
		{
			m_exitRequested = true;
		}

		inline bool GetExitRequested()
		{
			return m_exitRequested;
		}

	protected:
		void		 CalculateTime();
		void		 SetApplicationMode(ApplicationMode mode);
		virtual void LoadPlugins();
		virtual void UnloadPlugins();

	protected:
		CoreResourcesRegistry* m_coreResourceRegistry = nullptr;
		Engine				   m_engine;
		bool				   m_exitRequested = false;
		bool				   m_isIdleMode	   = false;
	};
} // namespace Lina

#endif
