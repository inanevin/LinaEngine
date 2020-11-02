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

#include "FPSDemo/FPSDemoLevel.hpp"
#include "ECS/ECS.hpp"
#include "Core/Application.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/TransformComponent.hpp"


namespace LinaEngine
{
	using namespace LinaEngine::ECS;

	ECSEntity m_playerEntity;
	ECSEntity m_cameraEntity;
	ECSRegistry* m_registry = nullptr;
	Transformation* m_playerTransform = nullptr;
	Transformation* m_cameraTransform = nullptr;

	bool FPSDemoLevel::Install(bool loadFromFile, const std::string& path, const std::string& levelName)
	{
		Level::Install(loadFromFile, path, levelName);

		m_registry = &Application::GetECSRegistry();

		return true;
	}

	void FPSDemoLevel::Initialize()
	{
		// Create player.
		m_playerEntity = m_registry->CreateEntity("Player");
		m_cameraEntity = m_registry->CreateEntity("FPS Cam");
		m_registry->AddChildToEntity(m_playerEntity, m_cameraEntity);
		
		// Get references
		m_playerTransform = &m_registry->get<TransformComponent>(m_playerEntity).transform;
		m_cameraTransform = &m_registry->get<TransformComponent>(m_cameraEntity).transform;

		// Set player hierarchy.
		m_cameraTransform->SetLocalLocation(Vector3(0, 1.8f, 0.0f));
	}

	void FPSDemoLevel::Tick(float delta)
	{
	}
}