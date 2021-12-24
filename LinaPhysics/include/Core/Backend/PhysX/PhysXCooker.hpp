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
Class: PhysXCooker



Timestamp: 12/24/2021 7:47:26 PM
*/

#pragma once

#ifndef PhysXCooker_HPP
#define PhysXCooker_HPP

// Headers here.
#include "EventSystem/Events.hpp"

namespace Lina::Graphics
{
	struct ModelNode;
	class Model;
}

namespace physx
{
	class PxFoundation;
}

namespace Lina::Physics
{
	class PhysXCooker
	{
		
	public:
		
		PhysXCooker() {};
		~PhysXCooker() {};
		void Initialize(ApplicationMode appMode, physx::PxFoundation* foundation);

	private:

		/// <summary>
		/// Uses the cooking library to create a convex mesh stream using the given vertices.
		/// Stream is placed into the given buffer data for custom serialization.
		/// </summary>
		void CookConvexMesh(std::vector<Vector3>& vertices, std::vector<uint8>& bufferData);

		void OnResourceLoadCompleted(Event::EResourceLoadCompleted ev);
		void CookModelNodeVertices(Lina::Graphics::ModelNode& node, Lina::Graphics::Model& model);

	private:

		ApplicationMode m_appMode;
	};
}

#endif
