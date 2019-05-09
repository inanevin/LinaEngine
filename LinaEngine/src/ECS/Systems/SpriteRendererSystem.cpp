/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: MeshRendererSystem
Timestamp: 4/27/2019 5:41:27 PM

*/

#include "LinaPch.hpp"
#include "ECS/Systems/SpriteRendererSystem.hpp"



namespace LinaEngine::ECS
{
	void SpriteRendererSystem::UpdateComponents(float delta, BaseECSComponent ** components)
	{
		TransformComponent* transform = (TransformComponent*)components[0];
		SpriteRendererComponent* sprite = (SpriteRendererComponent*)components[1];

		sprite->shader->SetSampler(m_SpriteSampler->GetSamplerName(), *sprite->texture, *m_SpriteSampler, 0, BindTextureMode::BINDTEXTURE_TEXTURE2D);
		m_RenderDevice->DrawSprite(m_FBO, sprite->shader->GetID(), m_VAO, *m_DrawParams, transform->transform.ToMatrix(), m_Projection, Vector3F(sprite->color.x, sprite->color.y, sprite->color.z));
	}
}

