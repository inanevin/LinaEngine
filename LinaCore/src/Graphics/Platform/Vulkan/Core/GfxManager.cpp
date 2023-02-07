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

#include "Graphics/Platform/Vulkan/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Core/WindowManager.hpp"
#include "Core/CoreResourcesRegistry.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Graphics/Resource/Shader.hpp"
#include "Graphics/Resource/Material.hpp"
#include "FileSystem/FileSystem.hpp"

namespace Lina
{
	void GfxManager::Initialize(const SystemInitializationInfo& initInfo)
	{
		LINA_TRACE("[Gfx Manager] -> Initialization.");

		m_backend.Initialize(initInfo, m_system->GetSubsystem<WindowManager>(SubsystemType::WindowManager)->GetWindow(LINA_MAIN_SWAPCHAIN));

		UserBinding globalBinding = UserBinding{
			.binding		 = 0,
			.descriptorCount = 1,
			.type			 = DescriptorType::UniformBuffer,
			.stages			 = {ShaderStage::Fragment, ShaderStage::Vertex},
		};

		UserBinding debugBinding = UserBinding{
			.binding		 = 1,
			.descriptorCount = 1,
			.type			 = DescriptorType::UniformBuffer,
			.stages			 = {ShaderStage::Fragment, ShaderStage::Vertex},
		};

		m_descriptorLayouts[DescriptorSetType::GlobalSet].device		= m_backend.GetDevice();
		m_descriptorLayouts[DescriptorSetType::GlobalSet].allocationCb	= m_backend.GetAllocationCb();
		m_descriptorLayouts[DescriptorSetType::GlobalSet].deletionQueue = &m_deletionQueue;
		m_descriptorLayouts[DescriptorSetType::GlobalSet].AddBinding(globalBinding).AddBinding(debugBinding).Create();

		UserBinding sceneBinding = UserBinding{
			.binding		 = 0,
			.descriptorCount = 1,
			.type			 = DescriptorType::UniformBuffer,
			.stages			 = {ShaderStage::Fragment, ShaderStage::Vertex},
		};

		UserBinding viewDataBinding = UserBinding{
			.binding		 = 1,
			.descriptorCount = 1,
			.type			 = DescriptorType::UniformBuffer,
			.stages			 = {ShaderStage::Vertex},
		};

		UserBinding lightDataBinding = UserBinding{
			.binding		 = 2,
			.descriptorCount = 1,
			.type			 = DescriptorType::UniformBuffer,
			.stages			 = {ShaderStage::Vertex},
		};

		UserBinding objDataBinding = UserBinding{
			.binding		 = 3,
			.descriptorCount = 1,
			.type			 = DescriptorType::StorageBuffer,
			.stages			 = {ShaderStage::Vertex},
		};

		m_descriptorLayouts[DescriptorSetType::PassSet].device		  = m_backend.GetDevice();
		m_descriptorLayouts[DescriptorSetType::PassSet].allocationCb  = m_backend.GetAllocationCb();
		m_descriptorLayouts[DescriptorSetType::PassSet].deletionQueue = &m_deletionQueue;
		m_descriptorLayouts[DescriptorSetType::PassSet].AddBinding(sceneBinding).AddBinding(viewDataBinding).AddBinding(lightDataBinding).AddBinding(objDataBinding).Create();

		// Pipeline layout
		m_globalAndPassLayout.device		= m_backend.GetDevice();
		m_globalAndPassLayout.allocationCb	= m_backend.GetAllocationCb();
		m_globalAndPassLayout.deletionQueue = &m_deletionQueue;
		m_globalAndPassLayout.AddDescriptorSetLayout(m_descriptorLayouts[DescriptorSetType::GlobalSet]).AddDescriptorSetLayout(m_descriptorLayouts[DescriptorSetType::PassSet]).Create();

		m_gpuUploader.Create();
	}

	void GfxManager::Shutdown()
	{
		LINA_TRACE("[Gfx Manager] -> Shutdown.");

		m_gpuUploader.Destroy();
		m_deletionQueue.Flush();
		m_backend.Shutdown();
	}

	void GfxManager::Tick()
	{
	}

	void GfxManager::Render()
	{
		m_gpuUploader.Poll();
	}
	void GfxManager::SyncData()
	{
	}

	void GfxManager::OnSystemEvent(ESystemEvent type, const Event& ev)
	{
		if (type & EVS_PostSystemInit)
		{
			auto					rm			= m_system->GetSubsystem<ResourceManager>(SubsystemType::ResourceManager);
			const Vector<StringID>& coreShaders = CoreResourcesRegistry::GetCoreShaders();

			for (const StringID& sid : coreShaders)
			{
				const String   matPath = "Resources/Core/Materials/" + FileSystem::RemoveExtensionFromPath(FileSystem::GetFilenameAndExtensionFromPath(rm->GetResource<Shader>(sid)->GetPath())) + ".linamat";
				const StringID matSid  = TO_SID(matPath);
				Material*	   mat	   = new Material(matPath, matSid, GetTypeID<Material>(), rm);
				mat->SetShader(sid);
				rm->AddUserManaged(mat, matSid);
				m_engineMaterials.push_back(mat);
			}
		}
		else if (type & EVS_PreSystemShutdown)
		{
			auto rm = m_system->GetSubsystem<ResourceManager>(SubsystemType::ResourceManager);

			for (auto mat : m_engineMaterials)
			{
				rm->RemoveUserManaged<Material>(mat->GetSID());
				delete mat;
			}

			m_engineMaterials.clear();
		}
	}

	DescriptorSetLayout& GfxManager::GetLayout(DescriptorSetType type)
	{
		return m_descriptorLayouts[type];
	}

} // namespace Lina
