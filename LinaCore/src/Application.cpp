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

#include "Core/Application.hpp"
#include "Core/ApplicationDelegate.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Common/Profiling/Profiler.hpp"
#include "Common/Profiling/MemoryTracer.hpp"
#include "Common/Platform/PlatformTime.hpp"
#include "Common/System/Plugin.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Renderers/SurfaceRenderer.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/CommonCore.hpp"

namespace Lina
{
	void Application::RegisterResourceData()
	{
		auto& rm = m_engine.GetResourceManager();

		rm.RegisterResourceType<Model>(50, {"glb"}, PackageType::Package1, 0);
		rm.RegisterResourceType<Shader>(25, {"linashader"}, PackageType::Package1, 0);
		rm.RegisterResourceType<Texture>(100, {"png", "jpeg", "jpg"}, PackageType::Package1, RTF_BINDLESS_RESOURCE);
		rm.RegisterResourceType<TextureSampler>(100, {"linasampler"}, PackageType::Package1, RTF_BINDLESS_RESOURCE);
		rm.RegisterResourceType<Font>(10, {"ttf", "otf"}, PackageType::Package1, 0);
		rm.RegisterResourceType<Material>(25, {"linamaterial"}, PackageType::Package1, RTF_BINDLESS_RESOURCE);
		rm.RegisterResourceType<EntityWorld>(25, {"linaworld"}, PackageType::Package1, 0);

		Vector<ResourceIdentifier> list;

		/* Priority Resources */
		list.push_back(ResourceIdentifier("Resources/Core/Textures/StubBlack.png", GetTypeID<Texture>(), 0, false, ResourceFlags::RF_PRIORITY));
		list.push_back(ResourceIdentifier(DEFAULT_SHADER_GUI_PATH, GetTypeID<Shader>(), 0, true, ResourceFlags::RF_PRIORITY));
		list.push_back(ResourceIdentifier(DEFAULT_FONT_PATH, GetTypeID<Font>(), 0, true, ResourceFlags::RF_PRIORITY));
		list.push_back(ResourceIdentifier(DEFAULT_SHADER_OBJECT_PATH, GetTypeID<Shader>(), 0, true, ResourceFlags::RF_PRIORITY));
		list.push_back(ResourceIdentifier(DEFAULT_SHADER_SKY_PATH, GetTypeID<Shader>(), 0, true, ResourceFlags::RF_PRIORITY));

		/* Core Resources */
		list.push_back(ResourceIdentifier(DEFAULT_SHADER_DEFERRED_LIGHTING_PATH, GetTypeID<Shader>(), 0, true, ResourceFlags::RF_CORE));
		list.push_back(ResourceIdentifier("Resources/Core/Textures/StubLinaLogo.png", GetTypeID<Texture>(), 0, true, ResourceFlags::RF_CORE));
		list.push_back(ResourceIdentifier("Resources/Core/Textures/StubLinaLogoWhite.png", GetTypeID<Texture>(), 0, true, ResourceFlags::RF_CORE));
		// list.push_back(ResourceIdentifier("Resources/Core/Models/LinaLogo.glb", GetTypeID<Model>(), 0, false, ResourceTag::Core));
		list.push_back(ResourceIdentifier("Resources/Core/Models/Cube.glb", GetTypeID<Model>(), 0, false, ResourceFlags::RF_CORE));
		list.push_back(ResourceIdentifier("Resources/Core/Models/Duck.glb", GetTypeID<Model>(), 0, false, ResourceFlags::RF_CORE));
		list.push_back(ResourceIdentifier("Resources/Core/Models/Sphere.glb", GetTypeID<Model>(), 0, false, ResourceFlags::RF_CORE));
		// list.push_back(ResourceIdentifier("Resources/Core/Models/LinaLogo.glb", GetTypeID<Model>(), 0, false, ResourceTag::Core));
		// list.push_back(ResourceIdentifier("Resources/Core/Models/Sphere.glb", GetTypeID<Model>(), 0, false, ResourceTag::Core));
		list.push_back(ResourceIdentifier("Resources/Core/Models/SkyCube.glb", GetTypeID<Model>(), 0, false, ResourceFlags::RF_CORE));
		list.push_back(ResourceIdentifier("Resources/Core/Textures/Checkered.png", GetTypeID<Texture>(), 0, false, ResourceFlags::RF_CORE));

		for (auto& r : list)
			r.sid = TO_SID(r.path);

		rm.RegisterAppResources(list);
	}

	bool Application::FillResourceCustomMeta(StringID sid, OStream& stream)
	{
		if (sid == DEFAULT_FONT_SID)
		{
			Font::Metadata customMeta = {
				.points = {{.size = 14, .dpiLimit = 10.1f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 14, .dpiLimit = 10.0f}},
				.isSDF	= false,

			};
			customMeta.SaveToStream(stream);
			return true;
		}
		// NOTE: 160, 380 is the glyph range for nunito sans

		if (sid == DEFAULT_TEXTURE_CHECKERED || sid == DEFAULT_TEXTURE_LINALOGO || sid == "Resources/Core/Textures/StubLinaLogoWhite.png"_hs)
		{
			Texture::Metadata meta = {
				.samplerSID = DEFAULT_SAMPLER_GUI_SID,
			};

			meta.SaveToStream(stream);
			return true;
		}

		if (sid == DEFAULT_SHADER_GUI_SID)
		{
			Shader::Metadata meta;

			meta.variants["RenderTarget"_hs] = ShaderVariant{
				.blendDisable = false,
				.depthTest	  = false,
				.depthWrite	  = false,
				.depthFormat  = LinaGX::Format::UNDEFINED,
				.targets	  = {{.format = DEFAULT_RT_FORMAT_HDR}},
				.cullMode	  = LinaGX::CullMode::None,
				.frontFace	  = LinaGX::FrontFace::CCW,
			};

			meta.variants["Swapchain"_hs] = ShaderVariant{
				.blendDisable = false,
				.depthTest	  = false,
				.depthWrite	  = false,
				.depthFormat  = LinaGX::Format::UNDEFINED,
				.targets	  = {{.format = DEFAULT_SWAPCHAIN_FORMAT}},
				.cullMode	  = LinaGX::CullMode::None,
				.frontFace	  = LinaGX::FrontFace::CCW,
			};

			meta.drawIndirectEnabled		  = true;
			meta.renderPassDescriptorType	  = RenderPassDescriptorType::Gui;
			meta.descriptorSetAllocationCount = 1;
			meta.SaveToStream(stream);
			return true;
		}

		if (sid == DEFAULT_SHADER_OBJECT_SID)
		{
			Shader::Metadata meta;
			meta.variants["RenderTarget"_hs] = ShaderVariant{
				.blendDisable = true,
				.depthTest	  = true,
				.depthWrite	  = true,
				.targets	  = {{.format = DEFAULT_RT_FORMAT}, {.format = DEFAULT_RT_FORMAT}, {.format = DEFAULT_RT_FORMAT}},
				.cullMode	  = LinaGX::CullMode::Back,
				.frontFace	  = LinaGX::FrontFace::CW,
			};

			meta.descriptorSetAllocationCount = 1;
			meta.drawIndirectEnabled		  = true;
			meta.renderPassDescriptorType	  = RenderPassDescriptorType::Main;
			meta.materialSize				  = sizeof(GPUMaterialDefaultObject);
			meta.SaveToStream(stream);
			return true;
		}

		if (sid == DEFAULT_SHADER_DEFERRED_LIGHTING_SID)
		{
			Shader::Metadata meta;
			meta.variants["RenderTarget"_hs] = ShaderVariant{
				.blendDisable = false,
				.depthTest	  = false,
				.depthWrite	  = false,
				.targets	  = {{.format = DEFAULT_RT_FORMAT_HDR}},
				.cullMode	  = LinaGX::CullMode::None,
				.frontFace	  = LinaGX::FrontFace::CW,
			};
			meta.descriptorSetAllocationCount = 1;
			meta.drawIndirectEnabled		  = false;
			meta.renderPassDescriptorType	  = RenderPassDescriptorType::Lighting;
			meta.materialSize				  = 0;
			meta.SaveToStream(stream);
			return true;
		}

		if (sid == DEFAULT_SHADER_SKY_SID)
		{
			Shader::Metadata meta;
			meta.variants["RenderTarget"_hs] = ShaderVariant{
				.blendDisable	   = true,
				.depthTest		   = true,
				.depthWrite		   = false,
				.targets		   = {{.format = DEFAULT_RT_FORMAT_HDR}},
				.depthOp		   = LinaGX::CompareOp::Equal,
				.cullMode		   = LinaGX::CullMode::Back,
				.frontFace		   = LinaGX::FrontFace::CW,
				.depthBiasEnable   = true,
				.depthBiasConstant = 5.0f,
			};

			meta.descriptorSetAllocationCount = 1;
			meta.drawIndirectEnabled		  = false;
			meta.renderPassDescriptorType	  = RenderPassDescriptorType::Lighting;
			meta.materialSize				  = sizeof(GPUMaterialDefaultSky);
			meta.SaveToStream(stream);
			return true;
		}

		return false;
	}

	void Application::Initialize(const SystemInitializationInfo& initInfo)
	{
		auto& resourceManager = m_engine.GetResourceManager();
		m_appDelegate		  = initInfo.appDelegate;
		m_appDelegate->m_app  = this;
		LINA_ASSERT(m_appDelegate != nullptr, "Application listener can not be empty!");

		// Setup
		{
			PlatformTime::Initialize();
			SystemInfo::SetAppStartCycles(PlatformTime::GetCPUCycles());
			SystemInfo::SetMainThreadID(SystemInfo::GetCurrentThreadID());
			SetFixedTimestep(10000);
			SetFixedTimestep(true);
			// SetFrameCap(16667);
			PROFILER_INIT;
			PROFILER_REGISTER_THREAD("Main");
			m_appDelegate->SetupPlatform(this);
		}

		// Resource registry
		{
			auto& resourceManager = m_engine.GetResourceManager();
			RegisterResourceData();
			m_appDelegate->RegisterAppResources(resourceManager);
		}

		// Pre-initialization
		{
			m_engine.PreInitialize(initInfo);
			resourceManager.LoadResources(resourceManager.GetPriorityResources());
			resourceManager.WaitForAll();
		}

		// Main window
		{
			auto window = m_engine.GetGfxManager().CreateApplicationWindow(LINA_MAIN_SWAPCHAIN, initInfo.appName, Vector2i::Zero, Vector2ui(initInfo.windowWidth, initInfo.windowHeight), static_cast<uint32>(initInfo.windowStyle));
			window->CenterPositionToCurrentMonitor();
			window->AddListener(this);
			window->SetVisible(true);
		}

		// Initialization
		{
			m_engine.Initialize(initInfo);
			auto& resourceManager = m_engine.GetResourceManager();
			m_coreResourcesTask	  = m_engine.GetResourceManager().LoadResources(resourceManager.GetCoreResources());
		}
	}

	void Application::LoadPlugins()
	{
		// PlatformProcess::LoadPlugin("GamePlugin.dll", m_engine.GetInterface(), &m_engine);
	}

	void Application::UnloadPlugins()
	{
		// PlatformProcess::UnloadPlugin("GamePlugin.dll", &m_engine);
	}

	void Application::PreTick()
	{
		PROFILER_FRAME_START();
		PROFILER_FUNCTION();

		// First launch, notify delegate of core resources loading.
		if (m_coreResourcesTask != -1 && m_engine.GetResourceManager().IsLoadTaskComplete(m_coreResourcesTask))
		{
			m_engine.GetResourceManager().WaitForAll();
			m_coreResourcesTask = -1;
			m_appDelegate->CoreResourcesLoaded();
		}

		m_engine.PreTick();
		m_appDelegate->PreTick();
	}

	void Application::Poll()
	{
		PROFILER_FUNCTION();
		m_engine.Poll();
	}

	void Application::Tick()
	{
		PROFILER_FUNCTION();

		m_engine.Tick();

		SystemInfo::SetFrames(SystemInfo::GetFrames() + 1);
		SystemInfo::SetAppTime(SystemInfo::GetAppTime() + SystemInfo::GetDeltaTime());

		// Yield-CPU check.
		if (!SystemInfo::GetAppHasFocus())
			PlatformTime::Sleep(0);
	}

	void Application::Shutdown()
	{
		m_engine.PreShutdown();
		m_engine.GetGfxManager().GetApplicationWindow(LINA_MAIN_SWAPCHAIN)->RemoveListener(this);
		m_engine.GetGfxManager().DestroyApplicationWindow(LINA_MAIN_SWAPCHAIN);
		m_engine.Shutdown();

		PROFILER_SHUTDOWN;
		m_appDelegate->Shutdown();
		delete m_appDelegate;
	}

	void Application::SetFrameCap(int64 microseconds)
	{
		SystemInfo::SetFrameCap(microseconds);
	}
	void Application::SetFixedTimestep(int64 microseconds)
	{
		SystemInfo::SetFixedTimestep(microseconds);
	}

	void Application::OnWindowClose()
	{
		m_exitRequested = true;
	}

} // namespace Lina
