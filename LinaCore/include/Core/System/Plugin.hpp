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

#include "Common/Event/SystemEventListener.hpp"
#include "Common/Data/String.hpp"

namespace Lina
{
	class PluginInterface;
	class EntityWorld;
	enum class PlayMode;

	class Plugin
	{
	public:
		Plugin(const String& path, void* platformHandle, PluginInterface* interface) : m_path(path), m_platformHandle(platformHandle), m_interface(interface){};
		virtual ~Plugin() = default;

		virtual void OnAttached() = 0;
		virtual void OnDetached() = 0;
		virtual void OnSetPlayMode(EntityWorld* world, PlayMode mode){};

		inline const String& GetPath()
		{
			return m_path;
		}

		inline void* GetPlatformHandle() const
		{
			return m_platformHandle;
		}

		inline void SetPath(const String& path)
		{
			m_path = path;
		}

	protected:
		void*			 m_platformHandle = nullptr;
		String			 m_path			  = "";
		PluginInterface* m_interface	  = nullptr;
	};

} // namespace Lina
