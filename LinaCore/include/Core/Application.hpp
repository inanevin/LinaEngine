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

#include "Common/JobSystem/JobSystem.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/GfxContext.hpp"
#include "Core/Graphics/GUI/GUIBackend.hpp"
#include "Core/Physics/PhysicsBackend.hpp"
#include "Core/Audio/AudioBackend.hpp"

namespace LinaGX
{
	class Window;
	class Instance;
} // namespace LinaGX

namespace Lina
{
	struct SystemInitializationInfo;
	class CoreResourcesRegistry;
	class ApplicationDelegate;
	class WorldRenderer;

	class Application
	{
	public:
		Application() : m_executor(2) {};
		~Application() {};

		bool Initialize(const SystemInitializationInfo& initInfo, String& errString);
		void Tick();
		void JoinRender();
		void Shutdown();

		void			DestroyApplicationWindow(StringID sid);
		LinaGX::Window* CreateApplicationWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2ui& size, uint32 style, LinaGX::Window* parentWindow = nullptr);
		LinaGX::Window* GetApplicationWindow(StringID sid);

		WorldRenderer* CreateAndLoadWorld(ProjectData* project, ResourceID id, LinaGX::Window* window);
		void		   UnloadWorld(WorldRenderer* wr);

		inline void Quit(const String& quitReason = "")
		{
			m_exitReason	= quitReason;
			m_exitRequested = true;
		}

		inline bool GetExitRequested()
		{
			return m_exitRequested;
		}

		inline void SetListener(ApplicationDelegate* listener)
		{
			m_appDelegate = listener;
		}

		inline ApplicationDelegate* GetAppDelegate()
		{
			return m_appDelegate;
		}

		static LinaGX::Instance* GetLGX()
		{
			return s_lgx;
		}

		inline JobExecutor& GetExecutor()
		{
			return m_executor;
		}

		inline ResourceManagerV2& GetResourceManager()
		{
			return m_resourceManager;
		}

		inline GUIBackend& GetGUIBackend()
		{
			return m_guiBackend;
		}

		inline GfxContext& GetGfxContext()
		{
			return m_gfxContext;
		}

		inline const String& GetExitReason() const
		{
			return m_exitReason;
		}

		inline PhysicsBackend& GetPhysicsBackend()
		{
			return m_physicsBackend;
		}

		inline static Log* GetLog()
		{
			return &s_log;
		}

	private:
		void Render();
		void CalculateTime();

	private:
		bool					 m_joinedRender		  = false;
		bool					 m_renderJoinPossible = false;
		static LinaGX::Instance* s_lgx;
		ApplicationDelegate*	 m_appDelegate	 = nullptr;
		bool					 m_exitRequested = false;
		bool					 m_isIdleMode	 = false;
		JobExecutor				 m_executor;
		ResourceManagerV2		 m_resourceManager;
		GUIBackend				 m_guiBackend;
		GfxContext				 m_gfxContext;
		String					 m_exitReason = "";
		PhysicsBackend			 m_physicsBackend;
		AudioBackend			 m_audioBackend;

		// Time
		int64 m_frameCapAccumulator = 0;

		static Log s_log;
	};
} // namespace Lina
