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

#include "Core/Resources/ResourceManager.hpp"
#include "Core/Application.hpp"
#include "Core/ApplicationDelegate.hpp"
#include "Core/Resources/Resource.hpp"
#include "Core/Application.hpp"
#include "Core/Resources/ResourceManagerListener.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/System/System.hpp"
#include "Common/Platform/PlatformTime.hpp"
#include "Common/Math/Math.hpp"
#include "Common/System/SystemInfo.hpp"

namespace Lina
{

	void ResourceManagerV2::Shutdown()
	{
		for (auto [tid, cache] : m_caches)
			delete cache;
	}

	void ResourceManagerV2::AddListener(ResourceManagerListener* listener)
	{
		m_listeners.push_back(listener);
	}

	void ResourceManagerV2::RemoveListener(ResourceManagerListener* listener)
	{
		m_listeners.erase(linatl::find_if(m_listeners.begin(), m_listeners.end(), [listener](ResourceManagerListener* list) -> bool { return list == listener; }));
	}

	void ResourceManagerV2::WaitForAll()
	{
		m_executor.Wait();

		for (ResourceLoadTask* task : m_loadTasks)
		{
			DispatchLoadTaskEvent(task);
			delete task;
		}

		m_loadTasks.clear();
	}

	void ResourceManagerV2::LoadResourcesFromFile(ApplicationDelegate* delegate, int32 taskID, const Vector<ResourceIdentifier>& identifiers, const String& baseCachePath, const String& projectPath)
	{
		Vector<ResourceIdentifier> idents = identifiers;

		for (auto& ident : idents)
		{
			ident.sid				 = TO_SID(ident.relativePath);
			auto			   it	 = m_caches.find(ident.tid);
			ResourceCacheBase* cache = nullptr;
			if (it == m_caches.end())
			{
				MetaType& type		= ReflectionSystem::Get().Resolve(ident.tid);
				void*	  ptr		= type.GetFunction<void*()>("CreateResourceCache"_hs)();
				cache				= static_cast<ResourceCacheBase*>(ptr);
				m_caches[ident.tid] = cache;
			}
			else
				cache = it->second;
			cache->Create(ident.relativePath, ident.sid);
		}

		for (ResourceManagerListener* listener : m_listeners)
			listener->OnResourceLoadStarted(taskID, idents);

		ResourceLoadTask* loadTask = new ResourceLoadTask();
		loadTask->id			   = taskID;
		loadTask->identifiers	   = idents;
		loadTask->startTime		   = PlatformTime::GetCPUCycles();
		loadTask->resources.resize(idents.size());
		m_loadTasks.push_back(loadTask);

		uint32 idx = 0;

		for (const auto& ident : idents)
		{
			loadTask->tf.emplace([idx, delegate, projectPath, ident, this, loadTask, baseCachePath, taskID]() {
				auto&	  cache			 = m_caches.at(ident.tid);
				Resource* res			 = cache->Get(ident.sid);
				loadTask->resources[idx] = res;

				String fullPath = FileSystem::GetRunningDirectory() + "/" + ident.relativePath;
				if (!FileSystem::FileOrPathExists(fullPath))
					fullPath = projectPath + "/" + ident.relativePath;

				const String metacachePath = baseCachePath + TO_STRING(TO_SID(fullPath)) + ".linameta";
				const bool	 exists		   = !baseCachePath.empty() && FileSystem::FileOrPathExists(metacachePath);

				if (exists && false)
				{
					IStream input = Serialization::LoadFromFile(metacachePath.c_str());
					res->LoadFromStream(input);
					input.Destroy();
				}
				else
				{
					// Some resources have preliminary/initial metadata.
					OStream metaStream;
					if (delegate->FillResourceCustomMeta(ident.sid, metaStream))
					{
						IStream in;
						in.Create(metaStream.GetDataRaw(), metaStream.GetCurrentSize());
						res->SetCustomMeta(in);
						in.Destroy();
					}
					metaStream.Destroy();

					res->LoadFromFile(fullPath.c_str());

					OStream metastream;
					res->SaveToStream(metastream);
					Serialization::SaveToFile(metacachePath.c_str(), metastream);
					metastream.Destroy();
				}
				LINA_TRACE("[Resource] -> Loaded resource: {0}", ident.relativePath);
				for (ResourceManagerListener* listener : m_listeners)
					listener->OnResourceLoaded(taskID, ident);
			});

			idx++;
		}

		m_executor.Run(loadTask->tf, [loadTask, this]() {
			loadTask->isCompleted.store(true);
			loadTask->endTime = PlatformTime::GetCPUCycles();
		});
	}

	void ResourceManagerV2::Poll()
	{
		for (Vector<ResourceLoadTask*>::iterator it = m_loadTasks.begin(); it != m_loadTasks.end();)
		{
			ResourceLoadTask* task = *it;
			if (task->isCompleted.load())
			{
				DispatchLoadTaskEvent(task);
				it = m_loadTasks.erase(it);
				delete task;
			}
			else
				++it;
		}
	}

	void ResourceManagerV2::DispatchLoadTaskEvent(ResourceLoadTask* task)
	{
		LINA_TRACE("[Resource Manager] -> Load task complete: {0} seconds", PlatformTime::GetDeltaSeconds64(task->startTime, task->endTime));
		for (ResourceManagerListener* listener : m_listeners)
			listener->OnResourceLoadEnded(task->id, task->resources);
	}

} // namespace Lina

/*

 // Packages
 HashMap<PackageType, Vector<ResourceIdentifier>> resourcesPerPackage;

 for (const auto& ident : identifiers)
 {
	 const PackageType pt = m_caches[ident.tid]->GetPackageType();
	 resourcesPerPackage[pt].push_back(ident);
 }

 for (auto& [pt, resourcesToLoad] : resourcesPerPackage)
 {
	 const String packagePath = GGetPackagePath(pt);
	 IStream         package     = Serialization::LoadFromFile(packagePath.c_str());

	 int loadedResources       = 0;
	 int totalResourcesSize = static_cast<int>(resourcesToLoad.size());

	 auto loadFunc = [this](IStream stream, ResourceIdentifier ident, ResourceLoadTask* loadTask) {
		 IStream      load    = stream;
		 auto&      cache = m_caches.at(ident.tid);
		 Resource* res    = cache->CreateResource(ident.sid, ident.path, this, ResourceOwner::ResourceManager);
		 res->m_flags    = ident.flags;
		 res->LoadFromStream(load);
		 res->Upload();

		 Event data;
		 data.pParams[0]       = &ident.path;
		 data.pParams[1]       = static_cast<void*>(loadTask);
		 data.uintParams[0] = ident.sid;
		 data.uintParams[1] = ident.tid;
		 m_system->DispatchEvent(EVS_ResourceLoaded, data);
		 stream.Destroy();
	 };

	 TypeID     tid  = 0;
	 StringID sid  = 0;
	 uint32     size = 0;
	 while (loadedResources < totalResourcesSize)
	 {
		 package >> tid;
		 package >> sid;
		 package >> size;

		 auto it = linatl::find_if(resourcesToLoad.begin(), resourcesToLoad.end(), [&](const ResourceIdentifier& ident) { return ident.tid == tid && ident.sid == sid; });

		 if (it != resourcesToLoad.end())
		 {
			 IStream stream;
			 stream.Create(package.GetDataCurrent(), size);
			 ResourceIdentifier ident = *it;
			 loadTask->tf.emplace([loadFunc, stream, ident, loadTask]() { loadFunc(stream, ident, loadTask); });

			 loadedResources++;
		 };

		 package.SkipBy(size);
	 }

	 package.Destroy();
 }
 */
