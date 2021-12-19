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
#include <entt/core/hashed_string.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/node.hpp>
#include <entt/meta/resolve.hpp>

namespace Lina::Editor
{

	typedef std::function<void(Lina::ECS::Entity)> ComponentFunction;
	typedef std::tuple<std::string, ComponentFunction, ComponentFunction> ComponentValueTuple;
	typedef std::function<entt::meta_any(Lina::ECS::Entity)> GetComponentFunction;
	typedef std::function<void(Lina::ECS::Entity)> RemoveComponentFunction;
	typedef std::function<void(Lina::ECS::Entity)> ResetComponentFunction;
	typedef std::function<void(Lina::ECS::Entity)> CopyComponentFunction;
	typedef std::map<std::string, std::vector<std::pair<std::string, Lina::ECS::TypeID>>> AddComponentMap;
	using namespace entt::literals;

	struct DrawData
	{
		DrawData() {};
		DrawData(const std::string& icon, const std::string& name, bool copyEnabled, bool pasteEnabled, bool resetEnabled, bool closeEnabled)
		{
			m_icon = icon;
			m_name = name;
			m_copyEnabled = copyEnabled;
			m_pasteEnabled = pasteEnabled;
			m_resetEnabled = resetEnabled;
			m_closeEnabled = closeEnabled;
			m_foldoutOpen = true;
		}

		std::string m_icon = "";
		std::string m_name = "";
		bool m_copyEnabled = true;
		bool m_pasteEnabled = true;
		bool m_resetEnabled = true;
		bool m_closeEnabled = true;
		bool m_foldoutOpen = true;
	};

	enum ComponentDrawFlags_
	{
		ComponentDrawFlags_None = 0,
		ComponentDrawFlags_NoRemove = 1 << 0,
		ComponentDrawFlags_NoReset = 1 << 1,
		ComponentDrawFlags_NoCopy = 1 << 2,
		ComponentDrawFlags_NoPaste = 1 << 3,
		ComponentDrawFlags_NoToggle = 1 << 4
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
		MaterialPathArray,
		ModelPath,
		ModelPathArray
	};

	class ComponentDrawer
	{

	public:

		ComponentDrawer();
		~ComponentDrawer() {};

		void Initialize();
		std::vector<std::string> GetEligibleComponents(Lina::ECS::Entity entity);
		void AddComponentToEntity(Lina::ECS::Entity entity, const std::string& comp);
		void SwapComponentOrder(Lina::ECS::TypeID id1, Lina::ECS::TypeID id2);
		void AddIDToDrawList(Lina::ECS::TypeID id);	
		void ClearDrawList();
		void DrawComponents(Lina::ECS::Entity entity);
		bool DrawComponentTitle(Lina::ECS::TypeID typeID, const char* title, const char* icon, bool* refreshPressed, bool* enabled, bool* foldoutOpen, const ImVec4& iconFolor = ImVec4(1, 1, 1, 1), const ImVec2& iconOffset = ImVec2(0, 0), bool cantDelete = false,  bool noRefresh = false);
	
		template<typename T>
		void RegisterComponentToDraw(Lina::ECS::TypeID typeID, const std::string& label, ComponentFunction drawFunction)
		{
			std::get<0>(m_componentFunctionsMap[typeID]) = label;
			std::get<1>(m_componentFunctionsMap[typeID]) = std::bind(&ComponentDrawer::ComponentAddFunction<T>, this, std::placeholders::_1);
			std::get<2>(m_componentFunctionsMap[typeID]) = drawFunction;
		}

	
		AddComponentMap GetCurrentAddComponentMap(Lina::ECS::Entity entity);
		void DrawDebugPointLight(Lina::ECS::Entity ent);
		void DrawDebugSpotLight(Lina::ECS::Entity ent);
		void DrawDebugDirectionalLight(Lina::ECS::Entity ent);
		void DrawComponent(Lina::ECS::TypeID tid, Lina::ECS::Entity ent);
		void DrawEntityData(Lina::ECS::Entity entity);
		void DrawEntityDataComponent(Lina::ECS::Entity entity);
		void DrawCameraComponent(Lina::ECS::Entity entity);
		void DrawFreeLookComponent(Lina::ECS::Entity entity);
		void DrawRigidbodyComponent(Lina::ECS::Entity entity);
		void DrawPointLightComponent(Lina::ECS::Entity entity);
		void DrawSpotLightComponent(Lina::ECS::Entity entity);
		void DrawDirectionalLightComponent(Lina::ECS::Entity entity);
		void DrawMeshRendererComponent( Lina::ECS::Entity entity);
		void DrawModelRendererComponent(Lina::ECS::Entity entity);
		void DrawSpriteRendererComponent( Lina::ECS::Entity entity);

	public:
		// Selected colilsion shape in editor.
		int m_currentCollisionShape = 0;

		// We use this instance to call DrawTitle method for defining the payload target while dragging & dropping component titles.
		static ComponentDrawer* s_activeInstance;


	private:

		template<typename T>
		void ComponentAddFunction(Lina::ECS::Entity entity)
		{
			auto* ecs = Lina::ECS::Registry::Get();
			ecs->emplace<T>(entity, T());
		}

		template<typename T>
		entt::meta_any GetComponentFunc(Lina::ECS::Entity ent)
		{
			return Lina::ECS::Registry::Get()->get<T>(ent);
		}

		template <typename T>
		void RemoveComponentFunc(Lina::ECS::Entity ent)
		{
			Lina::ECS::Registry::Get()->remove<T>(ent);
		}

		template <typename T>
		void ResetComponentFunc(Lina::ECS::Entity ent)
		{
			Lina::ECS::Registry::Get()->replace<T>(ent, T());
		}

		template <typename T>
		void CopyComponentFunc(Lina::ECS::Entity ent)
		{
			// m_copyBuffer = std::make_pair<Lina::ECS::Entity, Lina::ECS::TypeID>(ent, entt::type_id<T>().hash());
		}

		template <typename T>
		void PasteComponentFunc(Lina::ECS::Entity ent, T copy)
		{
			Lina::ECS::Registry::Get()->replace<T>(ent, copy);
		}

		template <typename T>
		void RegisterComponentFunctions(const std::string& title, const std::string& icon, bool removeEnabled = true, bool resetEnabled = true, bool copyEnabled = true, bool pasteEnabled = true)
		{
			Lina::ECS::TypeID tid = entt::type_id <T>().hash();
			m_getComponentFunctions[tid] = std::bind(&ComponentDrawer::GetComponentFunc<T>, this, std::placeholders::_1);
			m_removeComponentFunctions[tid] = std::bind(&ComponentDrawer::RemoveComponentFunc<T>, this, std::placeholders::_1);
			m_resetComponentFunctions[tid] = std::bind(&ComponentDrawer::ResetComponentFunc<T>, this, std::placeholders::_1);
			m_copyComponentFunctions[tid] = std::bind(&ComponentDrawer::CopyComponentFunc<T>, this, std::placeholders::_1);
			m_componentDrawData[tid] = DrawData(icon, title, removeEnabled, resetEnabled, copyEnabled, pasteEnabled);
		}


		template<typename Type>
		void RegisterComponentForEditor(char* title, char* icon, uint8 drawFlags, std::string category = "Default", bool canAddComponent = true)
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
		}

	private:

		std::map<Lina::ECS::TypeID, GetComponentFunction> m_getComponentFunctions;
		std::map<Lina::ECS::TypeID, RemoveComponentFunction> m_removeComponentFunctions;
		std::map<Lina::ECS::TypeID, ResetComponentFunction> m_resetComponentFunctions;
		std::map<Lina::ECS::TypeID, CopyComponentFunction> m_copyComponentFunctions;
		std::map<Lina::ECS::TypeID, DrawData> m_componentDrawData;

		std::map<Lina::ECS::TypeID, ComponentValueTuple> m_componentFunctionsMap;
		std::map<Lina::ECS::Entity, std::map<Lina::ECS::TypeID, bool>> m_foldoutStateMap;
		std::vector<Lina::ECS::TypeID> m_componentDrawList;

		// Category - vector of pairs - pair.first = component title, pair.second component id.
		AddComponentMap m_addComponentMap;


	};


}

#endif
