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

#include "ECS/Systems/FrustumSystem.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"

namespace Lina::ECS
{
	void FrustumSystem::Initialize()
	{
		BaseECSSystem::Initialize();
	}

	void FrustumSystem::UpdateComponents(float delta)
	{
	}

	void FrustumSystem::GetEntityBounds(Entity ent, Lina::Vector3& boundsPosition, Lina::Vector3& boundsHalfExtent)
	{
		MeshRendererComponent* mr = m_ecs->try_get<MeshRendererComponent>(ent);

		if (mr != nullptr)
		{
			EntityDataComponent& data = m_ecs->get<EntityDataComponent>(ent);
			const Vector3 entityLocation = data.GetLocation();
			const Vector3 vertexOffset = mr->m_totalVertexCenter * data.GetScale();
			const Vector3 offsetAddition = data.GetRotation().GetForward() * vertexOffset.z + data.GetRotation().GetRight() * vertexOffset.x + data.GetRotation().GetUp() * vertexOffset.y;
			boundsPosition = entityLocation + offsetAddition;
			boundsHalfExtent = mr->m_totalHalfBounds * data.GetScale() * data.GetRotation();
			return;
		}

		SpriteRendererComponent* sr = m_ecs->try_get<SpriteRendererComponent>(ent);
		if (sr != nullptr)
		{
			EntityDataComponent& data = m_ecs->get<EntityDataComponent>(ent);
			const Vector3 entityLocation = data.GetLocation();
			boundsPosition = entityLocation;
			boundsHalfExtent = Vector3(0.5f, 0.5f, 0.5f) * data.GetScale() * data.GetRotation();
			return;
		}

		LINA_ERR("Entity is not renderable! {0}", typeid(*this).name());
	}
}