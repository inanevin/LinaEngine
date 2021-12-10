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
Class: SpriteRendererSystem

Responsible for adding all the sprite renderers into a pool which is then
flushed to draw those renderers' data by the RenderEngine.

Timestamp: 10/1/2020 9:27:40 AM
*/
#pragma once

#ifndef SpriteRendererSystem_HPP
#define SpriteRendererSystem_HPP


#include "ECS/ECS.hpp"
#include "PackageManager/PAMRenderDevice.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/Mesh.hpp"

namespace Lina
{
	namespace Graphics
	{
		class RenderEngine;
		class Material;
	}
}

namespace Lina::ECS
{
	class SpriteRendererSystem : public BaseECSSystem
	{

		struct BatchModelData
		{
			std::vector<Matrix> m_models;
		};

	public:
		
		SpriteRendererSystem() {};
		~SpriteRendererSystem() {};
	
		void Construct(Registry& registry, Graphics::RenderEngine& renderEngineIn, RenderDevice& renderDeviceIn);
		virtual void UpdateComponents(float delta) override;

		void Render(Graphics::Material& material, const Matrix& transformIn);
		void Flush(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial = nullptr, bool completeFlush = true);

	private:
	
		Graphics::Mesh m_quadMesh;
		RenderDevice* s_renderDevice = nullptr;
		Graphics::RenderEngine* m_renderEngine = nullptr;
		std::map<Graphics::Material*, BatchModelData> m_renderBatch;
	};
}

#endif
