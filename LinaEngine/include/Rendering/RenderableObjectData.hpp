/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: RenderableObjectData
Timestamp: 5/4/2019 11:41:37 PM

*/

#pragma once

#ifndef RenderableObjectData_HPP
#define RenderableObjectData_HPP


#include "Rendering/Texture.hpp"
#include "Rendering/VertexArray.hpp"

namespace LinaEngine::Graphics
{
	class RenderableObjectData
	{
	public:

		RenderableObjectData() {};

		FORCEINLINE VertexArray& GetVertexArray()
		{
			return m_VertexArray;
		}

		FORCEINLINE Texture& GetTexture()
		{
			return m_Texture;
		}

	private:

		VertexArray m_VertexArray;
		Texture m_Texture;

	};
}


#endif