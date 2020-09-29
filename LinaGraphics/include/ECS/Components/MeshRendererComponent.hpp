/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: MeshRendererComponent
Timestamp: 4/14/2019 1:37:59 AM

*/

#pragma once

#ifndef RenderableMeshComponent_HPP
#define RenderableMeshComponent_HPP
#include "Core/SizeDefinitions.hpp"
#include "ECS/ECSComponent.hpp"



namespace LinaEngine
{
	namespace Graphics
	{
		class Mesh;
		class Material;
		class VertexArray;


	}

	
}

namespace LinaEngine::ECS
{
	struct MeshRendererComponent : public ECSComponent
	{
		int meshID = 0;
		int materialID = 0;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(meshID, materialID); // serialize things by passing them to the archive
		}
	};
}

#endif