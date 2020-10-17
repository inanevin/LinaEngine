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
Class: PropertiesPanel

The central system for drawing properties of any objects & files including materials, textures,
meshes, entities etc.

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
#include "Drawers/EntityDrawer.hpp"

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
		EntityDrawer m_entityDrawer;

		class LinaEngine::Graphics::RenderEngine* m_renderEngine;
		LinaEngine::ECS::ECSRegistry* m_ecs;
		DrawType m_currentDrawType = DrawType::NONE;

	};
}

#endif
