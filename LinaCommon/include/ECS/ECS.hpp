/* 
This file is a part of: Lina AudioEngine
https://github.com/inanevin/Lina

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: ECSSystem

Defines ECSRegistryOld wrapper, base ECS system class that defines functions for updating entity components
as well as an ECS Systems class responsible for iterating & calling update functions of containted systems.

Timestamp: 4/8/2019 5:28:34 PM
*/

#pragma once

#ifndef ECS_HPP
#define ECS_HPP

#include <linacommon_export.h>

#ifdef LinaCommon_EXPORTS
#define ENTT_DLL_EXPORT
#else
#define ENTT_API_IMPORT
#endif

#define ENTT_USE_ATOMIC
#include <entt/config/config.h>

#include <entt/entity/registry.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/snapshot.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <map>
#include <set>
#include <algorithm>
#include <execution>

namespace Lina::ECS
{
	typedef entt::entity Entity;
	typedef entt::id_type TypeID;
	typedef entt::snapshot Snapshot;
	typedef entt::continuous_loader SnapshotLoader;
	typedef entt::registry Registry;
	typedef entt::delegate<void(Registry&, cereal::PortableBinaryOutputArchive&)> ComponentSerializeFunction;
	typedef entt::delegate<void(Registry&, SnapshotLoader&, cereal::PortableBinaryInputArchive&)> ComponentDeserializeFunction;

#define ECS_GET entt::get

	extern void SerializeComponentsInRegistry(Registry& reg, cereal::PortableBinaryOutputArchive& archive);
	extern void DeserializeComponentsInRegistry(Registry& reg, cereal::PortableBinaryInputArchive& archive, SnapshotLoader& loader);

	template<typename T>
	TypeID GetTypeID()
	{
		return entt::type_info<T>::id();
	}
	
	template<typename Type>
	void SerializeComponent(Registry& registry, cereal::PortableBinaryOutputArchive& archive)
	{
		Snapshot(registry).component<Type>(archive);
	}

	template<typename Type>
	void DeserializeComponent(Registry& registry, SnapshotLoader& loader, cereal::PortableBinaryInputArchive& archive)
	{
		loader.component<Type>(archive);
	}

	template<typename T>
	void RegisterComponent()
	{
		g_serializeFunctions[GetTypeID<T>()].first.connect<&SerializeComponent<T>>();
		g_serializeFunctions[GetTypeID<T>()].second.connect<&DeserializeComponent<T>>();
	}


	extern LINACOMMON_EXPORT std::unordered_map<TypeID, std::pair<ComponentSerializeFunction, ComponentDeserializeFunction>> g_serializeFunctions;

}


#endif