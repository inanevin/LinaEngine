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

#ifndef ISubsystem_HPP
#define ISubsystem_HPP

#include "Common/Data/HashMap.hpp"

namespace Lina
{
	enum class SubsystemType
	{
		None = 0,
		GfxManager,
		AudioManager,
		WorldManager,
		Count
	};

	class System;
	struct SystemInitializationInfo;

	class Subsystem
	{
	public:
		virtual void PreInitialize(const SystemInitializationInfo& initInfo){};
		virtual void Initialize(const SystemInitializationInfo& initInfo){};
		virtual void PreTick(){};
		virtual void PreShutdown(){};
		virtual void Shutdown(){};

		inline SubsystemType GetType() const
		{
			return m_systemType;
		}

		inline System* GetSystem()
		{
			return m_system;
		}

	protected:
		Subsystem(System* sys, SubsystemType type);
		virtual ~Subsystem();

	protected:
		System*		  m_system	   = nullptr;
		SubsystemType m_systemType = SubsystemType::None;
	};
} // namespace Lina

#endif
