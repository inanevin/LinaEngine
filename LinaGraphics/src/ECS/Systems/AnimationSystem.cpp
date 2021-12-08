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

#include "ECS/Systems/AnimationSystem.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/AnimationComponent.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Rendering/Mesh.hpp"
#include <ozz/animation/runtime/sampling_job.h>
#include <ozz/animation/runtime/local_to_model_job.h>
#include <ozz/base/span.h>

namespace LinaEngine::ECS
{
	void AnimationSystem::Construct(ECSRegistry& registry, LinaEngine::Graphics::RenderEngine* renderEngine)
	{
		BaseECSSystem::Construct(registry);
		m_renderEngine = renderEngine;
	}

	static float test = 0.0f;

	void AnimationSystem::UpdateComponents(float delta)
	{
	
		auto view = m_ecs->view<MeshRendererComponent, AnimationComponent>();
		test += delta * 0.1f;

		if (test > 1.0f)
			test = 0.0f;
		
		for (auto entity : view)
		{
			MeshRendererComponent& mr = view.get<MeshRendererComponent>(entity);
			AnimationComponent& ar = view.get<AnimationComponent>(entity);

			Graphics::Mesh& mesh = Graphics::Mesh::GetMesh(mr.m_meshID);
			Graphics::Skeleton& skeleton = mesh.GetSkeleton();

			auto& meshAnimations = skeleton.GetAnimations();

			if (meshAnimations.find(ar.m_animationName) != meshAnimations.end())
			{
				Graphics::Animation* anim = meshAnimations[ar.m_animationName];

				ozz::animation::SamplingJob samplingJob;
				samplingJob.animation = &anim->GetAnim();
				samplingJob.cache = &skeleton.GetCache();
				samplingJob.ratio = test;
				samplingJob.output = ozz::make_span(skeleton.GetLocals());
				
				if (!samplingJob.Run()) {
					continue;
				}

				ozz::animation::LocalToModelJob toModelJob;
				toModelJob.skeleton = &skeleton.GetSkeleton();
				toModelJob.input = ozz::make_span(skeleton.GetLocals());
				toModelJob.output = make_span(skeleton.GetModels());
			
				if (!toModelJob.Run()) {
					continue;
				}

				
			}
		}
	}
}