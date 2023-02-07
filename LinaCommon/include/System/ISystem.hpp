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

#ifndef ISystem_HPP
#define ISystem_HPP

#include "Event/IEventDispatcher.hpp"
#include "System/ISubsystem.hpp"
#include "System/IPlugin.hpp"
#include "Data/Vector.hpp"
#include "Core/Common.hpp"
#include "Data/HashMap.hpp"

namespace Lina
{
	class ISubsystem;

	class ISystem : public IEventDispatcher
	{
	public:
		ISystem()		   = default;
		virtual ~ISystem() = default;

		void		AddSubsystem(ISubsystem* sub);
		void		RemoveSubsystem(ISubsystem* sub);
		void		AddPlugin(IPlugin* plugin);
		void		RemovePlugin(IPlugin* plugin);
		void		SetPhysicsUpdateRate(float rate);
		ISubsystem* GetSubsystem(SubsystemType type);

		inline void SetTimescale(float ts)
		{
			m_gameTimescale = ts;
		}

		virtual void Initialize(const SystemInitializationInfo& initInfo) = 0;
		virtual void Shutdown()											  = 0;
		virtual void PreTick()											  = 0;
		virtual void Tick()												  = 0;
		virtual void Quit()												  = 0;

		template <typename T> T* GetSubsystem(SubsystemType type)
		{
			return static_cast<T*>(m_subsystems[type]);
		}

		inline const SystemInitializationInfo& GetInitInfo()
		{
			return m_initInfo;
		}

	protected:
		SystemInitializationInfo			m_initInfo;
		HashMap<SubsystemType, ISubsystem*> m_subsystems;
		Vector<IPlugin*>					m_plugins;
		HashMap<IPlugin*, void*>			m_pluginHandles;
		float								m_gameTimescale		 = 1.0f;
		bool								m_firstRun			 = true;
		float								m_physicsUpdateRate	 = 0.01f;
		float								m_physicsAccumulator = 0.0f;
		int									m_fps				 = 0;
		int									m_frames			 = 0;
		float								m_fpsCounter		 = 0.0f;
	};
} // namespace Lina

#endif
