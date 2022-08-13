/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "FeatureRenderers/StaticMeshRenderer.hpp"

namespace Lina::Graphics
{
    void StaticMeshRenderer::Initialize()
    {
    }
    void StaticMeshRenderer::Shutdown()
    {
    }
} // namespace Lina::Graphics

/*
 m_renderables.clear();
        m_renderables.resize(m_fetchedData.size());

        glm::vec3 camPos = {0.f, 0.f, -350.f};

        static float _frameNumber = 0.0f;
        _frameNumber += 0.1f;

        glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
        // camera projection
        glm::mat4 projection = glm::perspective(glm::radians(70.f), 1200.0f / 1200.0f, 0.1f, 1000.0f);
        projection[1][1] *= -1;

        for (auto& renderable : m_renderables)
        {
            if (renderable.material != lastMaterial)
            {
                renderable.material->GetPipeline().Bind(buffer, PipelineBindPoint::Graphics);
                renderable.material = lastMaterial;
            }

            const glm::mat4 meshMatrix = projection * view * renderable.transform;

            Graphics::MeshPushConstants constants;
            constants.renderMatrix = meshMatrix;

            buffer.PushConstants(lastMaterial->GetPipeline()._layout, GetShaderStage(ShaderStage::Vertex), 0, sizeof(Graphics::MeshPushConstants), &constants);

            if (renderable.mesh != lastMesh)
            {
                buffer.Draw(static_cast<uint32>(renderable.mesh->GetVertices().size()), 1, 0, 0);
                lastMesh = renderable.mesh;
            }
        }
*/