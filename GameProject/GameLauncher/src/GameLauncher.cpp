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

#include "GameLauncher.hpp"
#include "SwapchainRenderer.hpp"
#include "Core/Lina.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/Platform/PlatformProcess.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/World/Components/CompAudio.hpp"
#include "Core/World/Components/CompWidget.hpp"
#include "Core/World/Components/CompLight.hpp"

namespace Lina
{

#define PACKAGE_0_PATH "LinaPackage0.linapkg"
#define PACKAGE_1_PATH "LinaPackage1.linapkg"

	SystemInitializationInfo Lina_GetInitInfo()
	{
		const SystemInitializationInfo outInfo = SystemInitializationInfo{
			.appName	  = "Lina Game",
			.windowWidth  = 800,
			.windowHeight = 600,
			.windowStyle  = WindowStyle::WindowedApplication,
			.appDelegate  = new GameLauncher(),
		};
		return outInfo;
	}

	bool Lina::GameLauncher::PreInitialize(String& errString)
	{
		m_pluginInterface.SetLog(m_app->GetLog());

		if (!LoadGamePlugin(errString))
			return false;

		if (!VerifyPackages(errString))
			return false;

		IStream stream = Serialization::LoadFromFile(PACKAGE_0_PATH);
		uint32	pjSize = 0;
		stream >> pjSize;
		m_project.LoadFromStream(stream);
		stream.Destroy();

		m_app->GetResourceManager().SetUsePackages(PACKAGE_0_PATH, PACKAGE_1_PATH);
		m_app->GetResourceManager().LoadResourcesFromProject(&m_project, {ENGINE_SHADER_SWAPCHAIN_ID}, NULL, 0);

		// Check for Game Plugin
		// Check for Resources

		return true;
	}

	bool GameLauncher::Initialize(String& err)
	{
		LinaGX::Window* main = m_app->GetApplicationWindow(LINA_MAIN_SWAPCHAIN);
		main->AddListener(this);
		m_window			= main;
		m_lgx				= m_app->GetLGX();
		m_swapchainRenderer = new SwapchainRenderer(m_app, m_lgx, main, false);
		return true;
	}

	void GameLauncher::PostInitialize()
	{
		const Vector<ResourceID>& worlds = m_project.GetSettingsPackaging().worldIDsToPack;
		if (!worlds.empty())
		{
			ResourceID first = worlds.at(0);
			m_app->CreateAndLoadWorld(&m_project, first, m_window);
		}
	}

	void GameLauncher::OnWorldLoaded(WorldRenderer* wr)
	{
		m_wr	= wr;
		m_world = m_wr->GetWorld();
		m_swapchainRenderer->SetWorldRenderer(m_wr);

		m_world->GetWorldCamera().SetPosition(Vector3(0, 0, 0));
		m_world->GetWorldCamera().Calculate(m_wr->GetSize());
	}

	void GameLauncher::OnWorldUnloaded(ResourceID id)
	{
		m_wr	= nullptr;
		m_world = nullptr;
		m_swapchainRenderer->SetWorldRenderer(nullptr);
	}

	void GameLauncher::PreTick()
	{
		m_swapchainRenderer->CheckVisibility();
	}

	void GameLauncher::Tick(float delta)
	{
		if(m_world)
			m_world->Tick(delta);

		if (m_wr)
			m_wr->Tick(delta);
		m_swapchainRenderer->Tick();
	}

	void GameLauncher::PreShutdown()
	{
		if (m_gamePlugin)
		{
			PlatformProcess::UnloadPlugin(m_gamePlugin);
		}

		if (m_wr)
			m_app->UnloadWorld(m_wr);

		m_app->GetApplicationWindow(LINA_MAIN_SWAPCHAIN)->RemoveListener(this);
		delete m_swapchainRenderer;
	}

	void GameLauncher::SyncRender()
	{
		if (m_wr)
			m_wr->SyncRender();
		m_swapchainRenderer->SyncRender();
	}

	void GameLauncher::Render(uint32 frameIndex)
	{
		if (m_wr)
		{
			m_wr->UpdateBuffers(frameIndex);
			m_wr->FlushTransfers(frameIndex);
			m_wr->Render(frameIndex);
			m_wr->CloseAndSend(frameIndex);
		}

		Vector<uint16> waitSemaphores;
		Vector<uint64> waitValues;

		if (m_wr)
		{
			const SemaphoreData& sem = m_wr->GetSubmitSemaphore(frameIndex);
			waitSemaphores.push_back(sem.GetSemaphore());
			waitValues.push_back(sem.GetValue());
		}

		if (!m_swapchainRenderer->GetIsVisible())
			return;

		LinaGX::CommandStream* cmd		 = m_swapchainRenderer->Render(frameIndex);
		uint8				   swapchain = m_swapchainRenderer->GetSwapchain();

		m_lgx->SubmitCommandStreams({
			.targetQueue	= m_lgx->GetPrimaryQueue(LinaGX::CommandType::Graphics),
			.streams		= &cmd,
			.streamCount	= static_cast<uint32>(1),
			.useWait		= !waitValues.empty(),
			.waitCount		= static_cast<uint32>(waitValues.size()),
			.waitSemaphores = waitSemaphores.data(),
			.waitValues		= waitValues.data(),
			.useSignal		= false,
		});

		m_lgx->Present({
			.swapchains		= &swapchain,
			.swapchainCount = static_cast<uint32>(1),
		});
	}

	void GameLauncher::OnWindowClose(LinaGX::Window* window)
	{
		m_app->Quit();
	}

	void GameLauncher::OnWindowSizeChanged(LinaGX::Window* window, const LinaGX::LGXVector2ui& sz)
	{
		m_app->JoinRender();

		if (m_wr)
			m_wr->Resize(sz);

		m_swapchainRenderer->OnWindowSizeChanged(window, sz);
	}

	void GameLauncher::OnWindowKey(LinaGX::Window* window, uint32 keycode, int32 scancode, LinaGX::InputAction inputAction)
	{
		if(window != m_window)
			return;

		if(m_world)
			m_world->GetInput().OnKey(keycode, scancode, inputAction);
	}

	void GameLauncher::OnWindowMouse(LinaGX::Window* window, uint32 button, LinaGX::InputAction inputAction)
	{
		if(window != m_window)
			return;

		if(m_world)
			m_world->GetInput().OnMouse(button, inputAction);
	}

	void GameLauncher::OnWindowMouseWheel(LinaGX::Window* window, float amt)
	{
		if(window != m_window)
			return;

		if(m_world)
			m_world->GetInput().OnMouseWheel(amt);
	}

	void GameLauncher::OnWindowMouseMove(LinaGX::Window* window, const LinaGX::LGXVector2& move)
	{
		if(window != m_window)
			return;

		if(m_world)
			m_world->GetInput().OnMouseMove(move);
	}

	void GameLauncher::OnWindowFocus(LinaGX::Window* window, bool gainedFocus)
	{
	}

	void GameLauncher::OnWindowHoverBegin(LinaGX::Window* window)
	{
	}

	void GameLauncher::OnWindowHoverEnd(LinaGX::Window* window)
	{
	}

	bool GameLauncher::LoadGamePlugin(String& errString)
	{
#ifdef LINA_PLATFORM_WINDOWS
		const String path = "GamePlugin.dll";
#endif

#ifdef LINA_PLATFORM_APPLE
		const String path = "libGamePlugin.dylib";
#endif

		if (!FileSystem::FileOrPathExists(path))
		{
			errString = "GamePlugin could not be found!";
			return false;
		}

		m_gamePlugin = PlatformProcess::LoadPlugin(path, &m_pluginInterface);
		if (!m_gamePlugin)
		{
			errString = "GamePlugin could not be loaded!";
			return false;
		}

		return true;
	}

	bool GameLauncher::VerifyPackages(String& errString)
	{
		if (!FileSystem::FileOrPathExists(PACKAGE_0_PATH))
		{
			errString = "Could not locate package LinaPackage0.linapkg";
			return false;
		}

		if (!FileSystem::FileOrPathExists(PACKAGE_1_PATH))
		{
			errString = "Could not locate package LinaPackage1.linapkg";
			return false;
		}

		return true;
	}
} // namespace Lina
