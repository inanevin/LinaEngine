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
Class: Player

FPS player entity for FPSDemo.

Timestamp: 11/3/2020 1:29:47 AM
*/

#pragma once

#ifndef Player_HPP
#define Player_HPP

// Headers here.
#include "ECS/ECS.hpp"
#include "Core/Layer.hpp"

namespace LinaEngine
{
	namespace ECS
	{
		class CameraComponent;
	}

	class Transformation;

	class Player : public Layer
	{
		
	public:
		
		Player() {};
		~Player() {};

		virtual void Attach() override;
		virtual void Detach() override;
		virtual void Tick(float deltaTime) override;
	
	private:
	
		ECS::ECSEntity m_playerEntity;
		ECS::ECSEntity m_cameraEntity;
		ECS::ECSRegistry* m_registry = nullptr;
		Transformation* m_playerTransform = nullptr;
		Transformation* m_cameraTransform = nullptr;
		ECS::CameraComponent* m_cameraComponent = nullptr;

	};
}

#endif
