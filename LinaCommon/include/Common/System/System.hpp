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

#ifndef System_HPP
#define System_HPP

#include "Common/Event/SystemEventDispatcher.hpp"
#include "Common/System/Subsystem.hpp"
#include "Common/Data/HashMap.hpp"
#include "Common/Common.hpp"
#include "Common/JobSystem/JobSystem.hpp"

namespace Lina
{
	class ISubsystem;
	class Application;

	class System : public SystemEventDispatcher
	{
	public:
		System(Application* app) : m_app(app){};
		virtual ~System() = default;

		void		 AddSubsystem(Subsystem* sub);
		void		 RemoveSubsystem(Subsystem* sub);
		Subsystem*	 CastSubsystem(SubsystemType type);
		virtual void Initialize(const SystemInitializationInfo& initInfo) = 0;
		virtual void Shutdown()											  = 0;
		virtual void PreTick()											  = 0;
		virtual void Poll()												  = 0;
		virtual void Tick()												  = 0;
		virtual void OnCriticalGfxError()								  = 0;

		template <typename T> T* CastSubsystem(SubsystemType type)
		{
			return (T*)(m_subsystems[type]);
		}

		inline const SystemInitializationInfo& GetInitInfo()
		{
			return m_initInfo;
		}

		inline JobExecutor* GetMainExecutor()
		{
			return &m_mainExecutor;
		}

		inline Application* GetApp()
		{
			return m_app;
		}

	protected:
		Application*					   m_app = nullptr;
		JobExecutor						   m_mainExecutor;
		SystemInitializationInfo		   m_initInfo;
		HashMap<SubsystemType, Subsystem*> m_subsystems;
	};
} // namespace Lina

#endif
