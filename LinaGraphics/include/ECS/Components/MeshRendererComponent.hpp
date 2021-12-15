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
Class: MeshRendererComponent
Timestamp: 4/14/2019 1:37:59 AM
*/

#pragma once

#ifndef RenderableMeshComponent_HPP
#define RenderableMeshComponent_HPP

#include "Utility/StringId.hpp"
#include "ECS/ECSComponent.hpp"
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

namespace Lina::ECS
{
	struct MeshRendererComponent : public ECSComponent
	{
		std::string m_modelPath = "";
		std::string m_materialPath = "";
		StringIDType m_modelID = -1;
		StringIDType m_materialID = -1;
		int m_meshIndex = -1;	// Index to define which mesh is this in the model's hierarchy. (parallel with the child entity indices under the model renderer component's entity).
		bool m_excludeFromDrawList = false;


		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_excludeFromDrawList, m_meshIndex, m_modelPath, m_materialPath, m_isEnabled);
		}
	};
}

#endif