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
Class: MeshRendererSystem

Responsible for adding all the mesh renderers into a pool which is then
flushed to draw those renderers' data by the RenderEngine.

Timestamp: 4/27/2019 5:38:44 PM
*/

#pragma once

#ifndef RenderableMeshSystem_HPP
#define RenderableMeshSystem_HPP

#include "ECS/ECSSystem.hpp"
#include "PackageManager/PAMRenderDevice.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "Rendering/RenderTarget.hpp"
#include "Rendering/VertexArray.hpp"
#include <queue>

namespace LinaEngine
{
	namespace Graphics
	{
		class RenderEngine;
		class Material;
		class Skeleton;

		struct BatchDrawData
		{
			Graphics::VertexArray* m_vertexArray;
			Graphics::Material* m_material;
			float m_distance;
		};

		struct BatchModelData
		{
			std::vector<Matrix> m_models;
			std::vector<Matrix> m_inverseTransposeModels;
			std::vector<Matrix> m_boneTransformations;
		};
	}
}

namespace LinaEngine::ECS
{
	struct TransformComponent;
	struct MeshRendererComponent;

	class MeshRendererSystem : public BaseECSSystem
	{

	public:

		typedef std::tuple<Graphics::BatchDrawData, Graphics::BatchModelData>  BatchPair;

		struct BatchComparison
		{
			bool const operator()(const BatchPair& lhs, const BatchPair& rhs) const
			{
				
				return std::get<0>(lhs).m_distance < std::get<0>(rhs).m_distance;
			}
		};

		struct BatchDrawDataComp
		{
			bool const operator()(const Graphics::BatchDrawData& lhs, const Graphics::BatchDrawData& rhs) const
			{
				return std::tie(lhs.m_vertexArray, lhs.m_material) < std::tie(rhs.m_vertexArray, rhs.m_material);
			}
		};

		MeshRendererSystem() {};

		void Construct(ECSRegistry& registry, Graphics::RenderEngine& renderEngineIn, RenderDevice& renderDeviceIn)
		{
			BaseECSSystem::Construct(registry);
			m_renderEngine = &renderEngineIn;
			s_renderDevice = &renderDeviceIn;
		}

		void RenderOpaque(Graphics::VertexArray& vertexArray, LinaEngine::Graphics::Skeleton& skeleton, Graphics::Material& material, const Matrix& transformIn);
		void RenderTransparent(Graphics::VertexArray& vertexArray, LinaEngine::Graphics::Skeleton& skeleton, Graphics::Material& material, const Matrix& transformIn, float priority);
		void FlushOpaque(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial = nullptr, bool completeFlush = true);
		void FlushTransparent(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial = nullptr, bool completeFlush = true);
	
		virtual void UpdateComponents(float delta) override;

		void FlushSingleRenderer(MeshRendererComponent& mrc, TransformComponent& transform, Graphics::DrawParams drawParams);

	private:

		RenderDevice* s_renderDevice = nullptr;
		Graphics::RenderEngine* m_renderEngine = nullptr;

		// Map & queue to see the list of same vertex array & textures to compress them into single draw call.
		std::map<Graphics::BatchDrawData, Graphics::BatchModelData, BatchDrawDataComp> m_opaqueRenderBatch;
		std::priority_queue<BatchPair, std::vector<BatchPair>, BatchComparison> m_transparentRenderBatch;
	};
}


#endif