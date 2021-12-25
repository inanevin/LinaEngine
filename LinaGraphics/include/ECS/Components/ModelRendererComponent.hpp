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
Class: ModelRendererComponent



Timestamp: 12/8/2021 12:27:21 PM
*/

#pragma once

#ifndef ModelRendererComponent_HPP
#define ModelRendererComponent_HPP

// Headers here.
#include "Math/Matrix.hpp"
#include "ECS/Component.hpp"
#include "Core/CommonECS.hpp"
#include "Utility/StringId.hpp"
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

namespace Lina
{
	namespace World
	{
		class Level;
	}

	namespace Graphics
	{
		class Model;
		class ModelNode;
		class Material;
	}
}

namespace Lina::Editor
{
	class ComponentDrawer;
}

namespace Lina::ECS
{
	class ModelNodeSystem;

	struct ModelRendererComponent : public Component
	{

		void SetModel(ECS::Entity parent, Graphics::Model& model);
		void RemoveModel(ECS::Entity parent);
		void SetMaterial(ECS::Entity parent, int materialIndex, const Graphics::Material& material);
		void RemoveMaterial(ECS::Entity parent, int materialIndex);
		bool GetGenerateMeshPivots() { return m_generateMeshPivots; }
		void RefreshHierarchy(ECS::Entity parent);
		StringIDType GetModelID() { return m_modelID; }
		std::string GetModelPath() { return m_modelPath; }
		std::vector<std::string> GetMaterialPaths() { return m_materialPaths; }

	private:

		void ProcessNode(ECS::Entity parent, const Matrix& parentTransform, Graphics::ModelNode& node, Graphics::Model& model, bool isRoot = false);
		void AddMeshRenderer(ECS::Entity targetEntity, const std::vector<int>& meshIndexes, Graphics::Model& model);

	private:

		friend class cereal::access;
		friend class ECS::ModelNodeSystem;
		friend class World::Level;
		friend class Editor::ComponentDrawer;

		StringIDType m_modelID = 0;
		std::string m_modelPath = "";
		std::string m_modelParamsPath = "";
		std::vector<std::string> m_materialPaths;
		int m_materialCount = -1;
		bool m_generateMeshPivots = false;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_modelPath, m_modelParamsPath, m_materialCount, m_materialPaths, m_isEnabled, m_generateMeshPivots);
		}
	
	};
}

#endif
