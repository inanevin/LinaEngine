/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

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
Class: ComponentDrawer

Responsible for drawing entity components by overriding Draw methods of the components.

Timestamp: 10/13/2020 2:34:21 PM
*/
#pragma once

#ifndef ComponentDrawer_HPP
#define ComponentDrawer_HPP

#include "Core/CommonECS.hpp"
#include "Core/SizeDefinitions.hpp"
#include "Core/EditorCommon.hpp"
#include "imgui/imgui.h"
#include <functional>
#include <map>
#include <tuple>
#include <entt/core/hashed_string.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/node.hpp>
#include <entt/meta/resolve.hpp>

namespace Lina::Editor
{

	typedef std::map<std::string, std::vector<std::pair<std::string, Lina::ECS::TypeID>>> AddComponentMap;
	using namespace entt::literals;

#define PROPS(LABEL, TYPE, TOOLTIP) std::make_pair("Label"_hs, LABEL), std::make_pair("Type"_hs, TYPE), std::make_pair("Tooltip"_hs, TOOLTIP)
#define PROPS_DEP(LABEL,TYPE, TOOLTIP, DISPLAYDEPENDENCY) std::make_pair("Label"_hs, LABEL), std::make_pair("Type"_hs, TYPE), std::make_pair("Tooltip"_hs, TOOLTIP), std::make_pair("DisplayDependency"_hs, DISPLAYDEPENDENCY)

	enum ComponentDrawFlags_
	{
		ComponentDrawFlags_None = 0,
		ComponentDrawFlags_NoRemove = 1 << 0,
		ComponentDrawFlags_NoReset = 1 << 1,
		ComponentDrawFlags_NoCopy = 1 << 2,
		ComponentDrawFlags_NoPaste = 1 << 3,
		ComponentDrawFlags_NoToggle = 1 << 4,
		ComponentDrawFlags_DisabledHeader = 1 << 5,
	};

	enum class ComponentVariableType
	{
		DragInt,
		DragFloat,
		Vector2,
		Vector3,
		Vector4,
		Color,
		Matrix,
		Checkmark,
		MaterialPath,
		MaterialArray,
		ModelPath,
		ModelPathArray
	};

	class ComponentDrawer
	{

	public:

		ComponentDrawer();
		~ComponentDrawer() {};

		void Initialize();
		void SwapComponentOrder(Lina::ECS::TypeID id1, Lina::ECS::TypeID id2);
		void AddIDToDrawList(Lina::ECS::TypeID id);
		void ClearDrawList();

		AddComponentMap GetCurrentAddComponentMap(Lina::ECS::Entity entity);
		void DrawDebugPointLight(Lina::ECS::Entity ent);
		void DrawDebugSpotLight(Lina::ECS::Entity ent);
		void DrawDebugDirectionalLight(Lina::ECS::Entity ent);
		void PushComponentToDraw(Lina::ECS::TypeID tid, Lina::ECS::Entity ent);
		void DrawAllComponents(Lina::ECS::Entity ent);

		void DrawEntityData(Lina::ECS::Entity ent, bool* transformDataOpen, bool* physicsDataOpen);
		void DrawComponent(Lina::ECS::TypeID tid, Lina::ECS::Entity ent);

	public:

		// Selected colilsion shape in editor.
		int m_currentCollisionShape = 0;

	private:

		void OnTransformPivotChanged(ETransformPivotChanged ev);

		template<typename Type>
		void RegisterComponentForEditor(char* title, char* icon, uint8 drawFlags, std::string category = "Default", bool canAddComponent = true, bool addValueChanged = false)
		{
			entt::meta<Type>().type().props(std::make_pair("Foldout"_hs, true), std::make_pair("Title"_hs, title), std::make_pair("Icon"_hs, icon), std::make_pair("DrawFlags"_hs, drawFlags),
				std::make_pair("Category"_hs, category));
			entt::meta<Type>().func<&Drawer_SetEnabled<Type>, entt::as_ref_t>("setEnabled"_hs);
			entt::meta<Type>().func<&Drawer_Get<Type>, entt::as_ref_t>("get"_hs);
			entt::meta<Type>().func<&Drawer_Reset<Type>, entt::as_ref_t>("reset"_hs);
			entt::meta<Type>().func<&Drawer_Remove<Type>, entt::as_ref_t>("remove"_hs);
			entt::meta<Type>().func<&Drawer_Copy, entt::as_ref_t>("copy"_hs);
			entt::meta<Type>().func<&Drawer_Paste<Type>, entt::as_ref_t>("paste"_hs);
			entt::meta<Type>().func<&Drawer_Has<Type>, entt::as_ref_t>("has"_hs);

			if (canAddComponent)
			{
				entt::meta<Type>().func<&Drawer_Add<Type>, entt::as_ref_t>("add"_hs);
				m_addComponentMap[category].push_back(std::make_pair<std::string, Lina::ECS::TypeID>(std::string(title), Lina::ECS::GetTypeID<Type>()));
			}

			if (addValueChanged)
			{
				entt::meta<Type>().func<&Drawer_ValueChanged<Type>, entt::as_ref_t>("valueChanged"_hs);
			}
		}

		void OnComponentOrderSwapped(EComponentOrderSwapped ev);

	private:

		AddComponentMap m_addComponentMap; 	// Category - vector of pairs - pair.first = component title, pair.second component id.
		std::map<Lina::ECS::Entity, std::map<Lina::ECS::TypeID, bool>> m_foldoutStateMap;
		std::vector<Lina::ECS::TypeID> m_componentDrawList;
		bool m_isTransformPivotGlobal = true;
	};


}

#endif
