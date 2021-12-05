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

namespace LinaEditor
{

	typedef std::function<void(LinaEngine::ECS::ECSRegistry&, LinaEngine::ECS::ECSEntity)> ComponentFunction;
	typedef std::tuple<std::string, ComponentFunction, ComponentFunction> ComponentValueTuple;

	class ComponentDrawer
	{

	public:

		ComponentDrawer();
		~ComponentDrawer() {};

		void Setup();
		std::vector<std::string> GetEligibleComponents(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity);
		void AddComponentToEntity(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity, const std::string& comp);
		void SwapComponentOrder(LinaEngine::ECS::ECSTypeID id1, LinaEngine::ECS::ECSTypeID id2);
		void AddIDToDrawList(LinaEngine::ECS::ECSTypeID id);	
		void ClearDrawList();
		void DrawComponents(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity);
		bool DrawComponentTitle(LinaEngine::ECS::ECSTypeID typeID, const char* title, const char* icon, bool* refreshPressed, bool* enabled, bool* foldoutOpen, const ImVec4& iconFolor = ImVec4(1, 1, 1, 1), const ImVec2& iconOffset = ImVec2(0, 0), bool alwaysEnabled = false);
	
		template<typename T>
		void RegisterComponentToDraw(LinaEngine::ECS::ECSTypeID typeID, const std::string& label, ComponentFunction drawFunction)
		{
			std::get<0>(m_componentFunctionsMap[typeID]) = label;
			std::get<1>(m_componentFunctionsMap[typeID]) = std::bind(&ComponentDrawer::ComponentAddFunction<T>, this, std::placeholders::_1, std::placeholders::_2);
			std::get<2>(m_componentFunctionsMap[typeID]) = drawFunction;
		}

		void DrawTransformComponent(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity);
		void DrawCameraComponent(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity);
		void DrawFreeLookComponent(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity);
		void DrawRigidbodyComponent(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity);
		void DrawPointLightComponent(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity);
		void DrawSpotLightComponent(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity);
		void DrawDirectionalLightComponent(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity);
		void DrawMeshRendererComponent(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity);
		void DrawSpriteRendererComponent(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity);

	public:
		// Selected colilsion shape in editor.
		int m_currentCollisionShape = 0;

		// We use this instance to call DrawTitle method for defining the payload target while dragging & dropping component titles.
		static ComponentDrawer* s_activeInstance;


	private:


		template<typename T>
		void ComponentAddFunction(LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::ECS::ECSEntity entity)
		{
			ecs.emplace<T>(entity, T());
		}

	private:

		std::map<LinaEngine::ECS::ECSTypeID, ComponentValueTuple> m_componentFunctionsMap;
		std::map<LinaEngine::ECS::ECSEntity, std::map<LinaEngine::ECS::ECSTypeID, bool>> m_foldoutStateMap;
		std::vector<LinaEngine::ECS::ECSTypeID> m_componentDrawList;


	};


}

#endif
