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
#include "Rendering/Mesh.hpp"
#include "ECS/ECS.hpp"
#include "Drawers/TextureDrawer.hpp"

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
	class GUILayer;
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

		PropertiesPanel(Vector2 position, Vector2 size, GUILayer& guiLayer) : EditorPanel(position, size, guiLayer) { };
		virtual ~PropertiesPanel() {};

		virtual void Draw(float frameTime) override;
		void Setup();


		void EntitySelected(LinaEngine::ECS::ECSEntity selectedEntity);
		void Texture2DSelected(LinaEngine::Graphics::Texture* texture, int id, std::string& path);

		FORCEINLINE void MeshSelected(LinaEngine::Graphics::Mesh* mesh, int id, std::string& path)
		{
			m_selectedMesh = mesh;
			m_currentDrawType = DrawType::MESH;
			m_selectedMeshID = id;
			m_selectedMeshPath = path;

			Graphics::MeshParameters& params = mesh->GetParameters();
			m_currentMeshParams = params;
		}

		FORCEINLINE void MaterialSelected(LinaEngine::Graphics::Material* material, int id, std::string& path)
		{
			m_selectedMaterial = material;
			m_selectedMaterialID = id;
			m_selectedMaterialPath = path;
			m_currentDrawType = DrawType::MATERIAL;
		}

		FORCEINLINE void Unselect()
		{
			m_selectedEntity = entt::null;
			m_selectedTexture = nullptr;
			m_selectedMesh = nullptr;
			m_selectedMaterial = nullptr;
			m_currentDrawType = DrawType::NONE;
		}

	private:

		// Drawing Meshes
		void DrawMeshProperties();

		// Drawing materials
		void DrawMaterialProperties();

	private:

		// Selected texture
		class LinaEngine::Graphics::Texture* m_selectedTexture;
		TextureDrawer m_textureDrawer;

		// Selected mesh
		class LinaEngine::Graphics::Mesh* m_selectedMesh;
		int m_selectedMeshID = 0;
		std::string m_selectedMeshPath;
		Graphics::MeshParameters m_currentMeshParams;

		// Selected material
		class LinaEngine::Graphics::Material* m_selectedMaterial;
		int m_selectedMaterialID = 0;
		std::string m_selectedMaterialPath;

		// Selected entity.
		LinaEngine::ECS::ECSEntity m_selectedEntity;
		bool m_copySelectedEntityName = true;

		class LinaEngine::Graphics::RenderEngine* m_renderEngine;
		LinaEngine::ECS::ECSRegistry* m_ecs;
		DrawType m_currentDrawType = DrawType::NONE;

	};
}

#endif
