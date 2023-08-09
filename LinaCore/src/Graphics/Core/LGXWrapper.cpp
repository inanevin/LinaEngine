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

#include "Graphics/Core/LGXWrapper.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Platform/LinaGXIncl.hpp"
#include "Core/Common.hpp"
#include "System/ISystem.hpp"
#include "Event/Event.hpp"

namespace Lina
{
	LGXWrapper::LGXWrapper(ISystem* sys) : ISubsystem(sys, SubsystemType::LGXWrapper)
	{
	}

	LINAGX_STRING FormatString(const char* fmt, va_list args)
	{
		// Determine the required size
		va_list args_copy;
		va_copy(args_copy, args);
		int size = vsnprintf(nullptr, 0, fmt, args_copy) + 1; // +1 for the null terminator
		va_end(args_copy);

		// Allocate a buffer and format the string
		std::vector<char> buffer(size);
		vsnprintf(buffer.data(), size, fmt, args);

		return std::string(buffer.data());
	}

	void LGXWrapper::PreInitialize(const SystemInitializationInfo& initInfo)
	{
		m_lgx = new LinaGX::Instance();

		typedef void (*LogCallback)(const char*, ...);

		LinaGX::Config.errorCallback = [](const char* err, ...) {
			va_list args;
			va_start(args, err);
			std::string formattedStr = FormatString(err, args);
			LINA_ERR(formattedStr.c_str());
			va_end(args);
		};

		LinaGX::Config.infoCallback = [](const char* info, ...) {
			va_list args;
			va_start(args, info);
			std::string formattedStr = FormatString(info, args);
			LINA_TRACE(formattedStr.c_str());
			va_end(args);
		};

		LinaGX::Config.logLevel = LinaGX::LogLevel::Verbose;

		LinaGX::BackendAPI api = LinaGX::BackendAPI::DX12;

#ifdef LINA_PLATFORM_APPLE
		api = LinaGX::BackendAPI::Metal;
#endif

		LinaGX::GPULimits limits = {
			.textureLimit		= 2048,
			.samplerLimit		= 512,
			.bufferLimit		= 1024,
			.maxSubmitsPerFrame = 30,
		};

		LinaGX::GPUFeatures features = {
			.enableBindless = true,
		};

		LinaGX::InitInfo lgxInitInfo = LinaGX::InitInfo{
			.api				   = api,
			.gpu				   = initInfo.preferredGPUType,
			.framesInFlight		   = FRAMES_IN_FLIGHT,
			.backbufferCount	   = BACK_BUFFER_COUNT,
			.gpuLimits			   = limits,
			.gpuFeatures		   = features,
			.checkForFormatSupport = {LinaGX::Format::R8G8B8A8_SRGB, LinaGX::Format::R32G32B32A32_SFLOAT, LinaGX::Format::D32_SFLOAT},
		};

		m_lgx->Initialize(lgxInitInfo);
	}

	void LGXWrapper::Initialize(const SystemInitializationInfo& initInfo)
	{
		m_lgx->GetInput().SetCallbackKey([&](uint32 key, int32 scanCode, LinaGX::InputAction action) {
			Event ev;
			ev.iParams[0] = key;
			ev.iParams[1] = static_cast<uint32>(action);
			m_system->DispatchEvent(EVS_Key, ev);
		});

		m_gfxManager = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
	}

	void LGXWrapper::PreShutdown()
	{
	}

	void LGXWrapper::Shutdown()
	{
		delete m_lgx;
	}

	LinaGX::Window* LGXWrapper::CreateApplicationWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2ui& size, bool isBorderless)
	{
		if (!m_gfxManager)
			m_gfxManager = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);

		m_lgx->Join();
		auto window = m_lgx->GetWindowManager().CreateApplicationWindow(sid, title, pos.x, pos.y, size.x, size.y, isBorderless ? LinaGX::WindowStyle::Borderless : LinaGX::WindowStyle::Windowed);
		m_gfxManager->CreateSurfaceRenderer(sid, window, size);
		return window;
	}

	void LGXWrapper::DestroyApplicationWindow(StringID sid)
	{
		m_lgx->Join();
		m_gfxManager->DestroySurfaceRenderer(sid);
		m_lgx->GetWindowManager().DestroyApplicationWindow(sid);
	}

	LinaGX::WindowManager* LGXWrapper::GetWindowManager()
	{
		return &m_lgx->GetWindowManager();
	}

	LinaGX::Input* LGXWrapper::GetInput()
	{
		return &m_lgx->GetInput();
	}
} // namespace Lina
