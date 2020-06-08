/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: PropertiesPanel
Timestamp: 6/7/2020 5:13:24 PM

*/
#pragma once

#ifndef PropertiesPanel_HPP
#define PropertiesPanel_HPP

#include "Panels/EditorPanel.hpp"
#include "Utility/EditorUtility.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "ECS/ECS.hpp"

namespace LinaEngine
{
	namespace Graphics
	{
		class RenderEngine;
		class Texture;
	}
}

namespace LinaEditor
{
	class PropertiesPanel : public EditorPanel
	{

		enum class DrawType
		{
			NONE,
			ENTITIES,
			TEXTURE2D,
			MATERIAL
		};

	public:

		PropertiesPanel(Vector2 position, Vector2 size, class GUILayer& guiLayer) : EditorPanel(position, size, guiLayer) {};
		virtual ~PropertiesPanel() {};

		virtual void Draw() override;
		void Setup();


		FORCEINLINE void EntitySelected(LinaEngine::ECS::ECSEntity selectedEntity) 
		{
			m_SelectedEntity = selectedEntity;  
			m_CurrentDrawType = DrawType::ENTITIES;
		}

		FORCEINLINE void Texture2DSelected(LinaEngine::Graphics::Texture* texture)
		{
			m_SelectedTexture = texture; 
			m_CurrentDrawType = DrawType::TEXTURE2D;
		}

		FORCEINLINE void Unselect()
		{
			m_SelectedEntity = entt::null;
			m_SelectedTexture = nullptr;
			m_CurrentDrawType = DrawType::NONE;
		}

	private:

		// Drawing Entities
		void DrawEntityProperties();
		void AddComponentToEntity(int componentID);
		void DrawComponents(LinaEngine::ECS::ECSEntity& entity);

		// Drawing textures
		void DrawTextureProperties();
		int GetSamplerFilterID(Graphics::SamplerFilter filter);
		int GetWrapModeID(Graphics::SamplerWrapMode wrapMode);
		Graphics::SamplerFilter GetSamplerFilterFromID(int id);
		Graphics::SamplerWrapMode GetWrapModeFromID(int id);
	private:

		class LinaEngine::Graphics::Texture* m_SelectedTexture;
		LinaEngine::ECS::ECSRegistry* m_ECS;
		LinaEngine::ECS::ECSEntity m_SelectedEntity;
		DrawType m_CurrentDrawType = DrawType::NONE;

	};
}

#endif
