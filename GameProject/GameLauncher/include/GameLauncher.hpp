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

#include "Core/ApplicationDelegate.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/System/PluginInterface.hpp"
#include <LinaGX/Core/WindowListener.hpp>

namespace LinaGX
{
	struct LGXVector2;
	class Instance;
	class Window;
} // namespace LinaGX

namespace Lina
{
	class SwapchainRenderer;
	class EntityWorld;
	class Plugin;

	class GameLauncher : public ApplicationDelegate, public LinaGX::WindowListener
	{
	public:
		virtual bool PreInitialize(String& errString) override;
		virtual bool Initialize(String& errString) override;
		virtual void OnWorldLoaded(WorldRenderer* wr) override;
		virtual void OnWorldUnloaded(ResourceID id) override;
		virtual void PostInitialize() override;
		virtual void PreTick() override;
		virtual void Tick(float delta) override;
		virtual void PreShutdown() override;
		virtual void SyncRender() override;
		virtual void Render(uint32 frameIndex) override;
		virtual void OnWindowClose(LinaGX::Window* window) override;
		virtual void OnWindowSizeChanged(LinaGX::Window* window, const LinaGX::LGXVector2ui&) override;
		virtual void OnWindowKey(LinaGX::Window* window, uint32 keycode, int32 scancode, LinaGX::InputAction inputAction) override;
		virtual void OnWindowMouse(LinaGX::Window* window, uint32 button, LinaGX::InputAction inputAction) override;
		virtual void OnWindowMouseWheel(LinaGX::Window* window, float amt) override;
		virtual void OnWindowMouseMove(LinaGX::Window* window, const LinaGX::LGXVector2&) override;
		virtual void OnWindowFocus(LinaGX::Window* window, bool gainedFocus) override;
		virtual void OnWindowHoverBegin(LinaGX::Window* window) override;
		virtual void OnWindowHoverEnd(LinaGX::Window* window) override;

	private:
		bool LoadGamePlugin(String& errString);
		bool VerifyPackages(String& errString);

	private:
		Plugin*			   m_gamePlugin		   = nullptr;
		PluginInterface	   m_pluginInterface   = {};
		LinaGX::Window*	   m_window			   = nullptr;
		WorldRenderer*	   m_wr				   = nullptr;
		EntityWorld*	   m_world			   = nullptr;
		SwapchainRenderer* m_swapchainRenderer = nullptr;
		ProjectData		   m_project		   = {};
		LinaGX::Instance*  m_lgx			   = nullptr;
	};
} // namespace Lina