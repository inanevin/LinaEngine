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

#include "ECS/ECS.hpp"
#include "imgui/imgui.h"
#include <functional>
#include <map>
#include <tuple>

namespace Lina::Editor
{

	typedef std::function<void(Lina::ECS::Registry&, Lina::ECS::Entity)> ComponentFunction;
	typedef std::tuple<std::string, ComponentFunction, ComponentFunction> ComponentValueTuple;

	class ComponentDrawer
	{

	public:

		ComponentDrawer();
		~ComponentDrawer() {};

		void Setup();
		std::vector<std::string> GetEligibleComponents(Lina::ECS::Registry& ecs, Lina::ECS::Entity entity);
		void AddComponentToEntity(Lina::ECS::Registry& ecs, Lina::ECS::Entity entity, const std::string& comp);
		void SwapComponentOrder(Lina::ECS::TypeID id1, Lina::ECS::TypeID id2);
		void AddIDToDrawList(Lina::ECS::TypeID id);	
		void ClearDrawList();
		void DrawComponents(Lina::ECS::Registry& ecs, Lina::ECS::Entity entity);
		bool DrawComponentTitle(Lina::ECS::TypeID typeID, const char* title, const char* icon, bool* refreshPressed, bool* enabled, bool* foldoutOpen, const ImVec4& iconFolor = ImVec4(1, 1, 1, 1), const ImVec2& iconOffset = ImVec2(0, 0), bool cantDelete = false,  bool noRefresh = false);
	
		template<typename T>
		void RegisterComponentToDraw(Lina::ECS::TypeID typeID, const std::string& label, ComponentFunction drawFunction)
		{
			std::get<0>(m_componentFunctionsMap[typeID]) = label;
			std::get<1>(m_componentFunctionsMap[typeID]) = std::bind(&ComponentDrawer::ComponentAddFunction<T>, this, std::placeholders::_1, std::placeholders::_2);
			std::get<2>(m_componentFunctionsMap[typeID]) = drawFunction;
		}

		void DrawEntityDataComponent(Lina::ECS::Registry& ecs, Lina::ECS::Entity entity);
		void DrawCameraComponent(Lina::ECS::Registry& ecs, Lina::ECS::Entity entity);
		void DrawFreeLookComponent(Lina::ECS::Registry& ecs, Lina::ECS::Entity entity);
		void DrawRigidbodyComponent(Lina::ECS::Registry& ecs, Lina::ECS::Entity entity);
		void DrawPointLightComponent(Lina::ECS::Registry& ecs, Lina::ECS::Entity entity);
		void DrawSpotLightComponent(Lina::ECS::Registry& ecs, Lina::ECS::Entity entity);
		void DrawDirectionalLightComponent(Lina::ECS::Registry& ecs, Lina::ECS::Entity entity);
		void DrawMeshRendererComponent(Lina::ECS::Registry& ecs, Lina::ECS::Entity entity);
		void DrawModelRendererComponent(Lina::ECS::Registry& ecs, Lina::ECS::Entity entity);
		void DrawSpriteRendererComponent(Lina::ECS::Registry& ecs, Lina::ECS::Entity entity);

	public:
		// Selected colilsion shape in editor.
		int m_currentCollisionShape = 0;

		// We use this instance to call DrawTitle method for defining the payload target while dragging & dropping component titles.
		static ComponentDrawer* s_activeInstance;


	private:


		template<typename T>
		void ComponentAddFunction(Lina::ECS::Registry& ecs, Lina::ECS::Entity entity)
		{
			ecs.emplace<T>(entity, T());
		}

	private:

		std::map<Lina::ECS::TypeID, ComponentValueTuple> m_componentFunctionsMap;
		std::map<Lina::ECS::Entity, std::map<Lina::ECS::TypeID, bool>> m_foldoutStateMap;
		std::vector<Lina::ECS::TypeID> m_componentDrawList;


	};


}

#endif
