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

#include "Common/Data/Vector.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Event/SystemEventListener.hpp"

namespace LinaGX
{
	class Window;
	class CommandStream;
} // namespace LinaGX

namespace Lina
{
	class Application;
	class System;
	class SemaphoreData;
	class WorldRenderer;

	class ApplicationDelegate : public SystemEventListener
	{
	public:
		ApplicationDelegate()		   = default;
		virtual ~ApplicationDelegate() = default;

		// Loop
		virtual bool PreInitialize(String& errString)
		{
			return true;
		};
		virtual bool Initialize(String& errString)
		{
			return true;
		};
		virtual void PostInitialize() {};
		virtual void PreTick() {};
		virtual void Tick(float delta) {};
		virtual void Render(uint32 frameIndex) {};
		virtual void SyncRender() {};
		virtual void PreShutdown() {};
		virtual void Shutdown() {};
		virtual void OnWindowSizeChanged(LinaGX::Window* window, const Vector2ui& size) {};
		virtual void OnWorldLoaded(WorldRenderer* wr) {};
		virtual void OnWorldUnloaded(ResourceID id) {};

		// Resources
		virtual bool FillResourceCustomMeta(ResourceID id, OStream& stream)
		{
			return false;
		};

		inline Application* GetApp() const
		{
			return m_app;
		}

	protected:
		friend class Application;

		Application* m_app = nullptr;
	};
} // namespace Lina
