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
#include "ECS/ECSComponent.hpp"
#include "ECS/ECSSystem.hpp"
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

namespace LinaEngine
{
	namespace World
	{
		class Level;
	}

	namespace Graphics
	{
		class Model;
		class Material;
	}
}

namespace LinaEditor
{
	class ComponentDrawer;
}

namespace LinaEngine::ECS
{
	class MeshRendererSystem;

	struct ModelRendererComponent : public ECSComponent
	{

		void SetModel(ECS::ECSRegistry& reg, ECS::ECSEntity parent, Graphics::Model& model);
		void RemoveModel(ECS::ECSRegistry& reg, ECS::ECSEntity parent);
		void SetMaterial(ECS::ECSRegistry& reg, ECS::ECSEntity parent, int materialIndex, const Graphics::Material& material);
		void RemoveMaterial(ECS::ECSRegistry& reg, ECS::ECSEntity parent, int materialIndex);
		

	private:

		friend class cereal::access;
		friend class ECS::MeshRendererSystem;
		friend class LinaEngine::World::Level;
		friend class LinaEditor::ComponentDrawer;

		int m_modelID = 0;
		std::string m_modelPath = "";
		std::string m_modelParamsPath = "";
		std::vector<std::string> m_materialPaths;
		int m_materialCount = -1;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_modelPath, m_modelParamsPath, m_materialCount, m_materialPaths, m_isEnabled);
		}
	
	};
}

#endif
