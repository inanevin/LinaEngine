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
#include "Rendering/Mesh.hpp"
#include "ECS/ECS.hpp"

namespace LinaEngine
{
	namespace Graphics
	{
		class RenderEngine;
		class Mesh;
		class Texture;
		class Material;
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
			MESH,
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

		void Texture2DSelected(LinaEngine::Graphics::Texture* texture, int id, std::string& path);

		FORCEINLINE void MeshSelected(LinaEngine::Graphics::Mesh* mesh, int id, std::string& path)
		{
			m_SelectedMesh = mesh;
			m_CurrentDrawType = DrawType::MESH;
			m_SelectedMeshID = id;
			m_SelectedMeshPath = path;

			Graphics::MeshParameters& params = mesh->GetParameters();
			m_CurrentMeshParams = params;
		}

		FORCEINLINE void MaterialSelected(LinaEngine::Graphics::Material* material, int id, std::string& path)
		{
			m_SelectedMaterial = material;
			m_SelectedMaterialID = id;
			m_SelectedMaterialPath = path;
			m_CurrentDrawType = DrawType::MATERIAL;
		}

		FORCEINLINE void Unselect()
		{
			m_SelectedEntity = entt::null;
			m_SelectedTexture = nullptr;
			m_SelectedMesh = nullptr;
			m_SelectedMaterial = nullptr;
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

		// Drawing Meshes
		void DrawMeshProperties();

		// Drawing materials
		void DrawMaterialProperties();

	private:

		// Selected texture
		class LinaEngine::Graphics::Texture* m_SelectedTexture;
		int m_SelectedTextureID = 0;
		std::string m_SelectedTexturePath;
		int m_CurrentInternalPF = 0;
		int m_CurrentPF = 0;
		int m_CurrentMinFilter = 0;
		int m_CurrentMagFilter = 0;
		int m_CurrentWrapS = 0;
		int m_CurrentWrapR = 0;
		int m_CurrentWrapT = 0;
		bool m_CurrentGenerateMips = 0;
		int m_CurrentAnisotropy= 0;

		// Selected colilsion shape
		int m_currentCollisionShape = 0;

		// Selected mesh
		class LinaEngine::Graphics::Mesh* m_SelectedMesh;
		int m_SelectedMeshID = 0;
		std::string m_SelectedMeshPath;
		Graphics::MeshParameters m_CurrentMeshParams;

		// Selected material
		class LinaEngine::Graphics::Material* m_SelectedMaterial;
		int m_SelectedMaterialID = 0;
		std::string m_SelectedMaterialPath;

		class LinaEngine::Graphics::RenderEngine* m_RenderEngine;
		LinaEngine::ECS::ECSRegistry* m_ECS;
		LinaEngine::ECS::ECSEntity m_SelectedEntity;
		DrawType m_CurrentDrawType = DrawType::NONE;

	};
}

#endif
