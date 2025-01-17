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

#include "Core/Graphics/Renderers/ShapeRenderer.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Graphics/ResourceUploadQueue.hpp"

namespace Lina
{

	void ShapeRenderer::Initialize()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			PerFrameData& pfd = m_pfd[i];
			pfd.vtxBuffer.Create(LinaGX::ResourceTypeHint::TH_VertexBuffer, sizeof(Line3DVertex) * 120000, "ShapeRenderer Vtx");
			pfd.idxBuffer.Create(LinaGX::ResourceTypeHint::TH_IndexBuffer, sizeof(uint16) * 200000, "ShapeRenderer Idx");
		}
	}

	void ShapeRenderer::Shutdown()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			PerFrameData& pfd = m_pfd[i];
			pfd.vtxBuffer.Destroy();
			pfd.idxBuffer.Destroy();
		}
	}

	void ShapeRenderer::SyncRender()
	{
		m_gpuDrawData = m_cpuDrawData;
		m_cpuDrawData.vertices.resize(0);
		m_cpuDrawData.indices.resize(0);
	}

	void ShapeRenderer::AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue)
	{
		PerFrameData& pfd = m_pfd[frameIndex];
		pfd.vtxBuffer.BufferData(0, (uint8*)m_gpuDrawData.vertices.data(), sizeof(Line3DVertex) * m_gpuDrawData.vertices.size());
		pfd.idxBuffer.BufferData(0, (uint8*)m_gpuDrawData.indices.data(), sizeof(uint16) * m_gpuDrawData.indices.size());
		queue.AddBufferRequest(&pfd.vtxBuffer);
		queue.AddBufferRequest(&pfd.idxBuffer);
	}

	void ShapeRenderer::StartBatch()
	{
		m_startIndex  = static_cast<uint32>(m_cpuDrawData.indices.size());
		m_startVertex = static_cast<uint32>(m_cpuDrawData.vertices.size());
	}

	void ShapeRenderer::SubmitBatch(RenderPass& pass, uint32 pushConstantValue, uint32 shaderHandle)
	{
		const uint32 ic = static_cast<uint32>(m_cpuDrawData.indices.size()) - m_startIndex;
		if (ic == 0)
			return;

		const RenderPass::InstancedDraw draw = {
			.vertexBuffers = {&m_pfd[0].vtxBuffer, &m_pfd[1].vtxBuffer},
			.indexBuffers  = {&m_pfd[0].idxBuffer, &m_pfd[1].idxBuffer},
			.vertexSize	   = sizeof(Line3DVertex),
			.shaderHandle  = shaderHandle,
			.baseVertex	   = 0,
			.baseIndex	   = m_startIndex,
			.indexCount	   = ic,
			.instanceCount = 1,
			.pushConstant  = pushConstantValue,
		};

		pass.AddDrawCall(draw);
	}

	void ShapeRenderer::DrawLine3D(const Vector3& p1, const Vector3& p2, float thickness, const ColorGrad& color)
	{
		const uint16 sz = static_cast<uint16>(m_cpuDrawData.vertices.size());

		m_cpuDrawData.vertices.push_back({
			.position	  = p1,
			.nextPosition = p2,
			.color		  = color.start,
			.direction	  = 1.0f * thickness,
		});

		m_cpuDrawData.vertices.push_back({
			.position	  = p1,
			.nextPosition = p2,
			.color		  = color.start,
			.direction	  = -1.0f * thickness,
		});

		m_cpuDrawData.vertices.push_back({
			.position	  = p2,
			.nextPosition = p1,
			.color		  = color.end,
			.direction	  = 1.0f * thickness,
		});

		m_cpuDrawData.vertices.push_back({
			.position	  = p2,
			.nextPosition = p1,
			.color		  = color.end,
			.direction	  = -1.0f * thickness,
		});

		m_cpuDrawData.indices.push_back(sz + 0);
		m_cpuDrawData.indices.push_back(sz + 1);
		m_cpuDrawData.indices.push_back(sz + 2);
		m_cpuDrawData.indices.push_back(sz + 2);
		m_cpuDrawData.indices.push_back(sz + 3);
		m_cpuDrawData.indices.push_back(sz);
	}

	void ShapeRenderer::DrawWireframeCube3D(const Vector3& center, const Vector3& extents, float thickness, const ColorGrad& color)
	{
		const Vector3 p0 = center - extents;
		const Vector3 p1 = p0 + Vector3(0, extents.y * 2, 0);
		const Vector3 p2 = p1 + Vector3(0, 0, extents.z * 2);
		const Vector3 p3 = p2 + Vector3(0, -extents.y * 2, 0);
		DrawLine3D(p0, p1, thickness, color);
		DrawLine3D(p1, p2, thickness, color);
		DrawLine3D(p2, p3, thickness, color);
		DrawLine3D(p0, p3, thickness, color);

		const Vector3 pr0 = p0 + Vector3(extents.x * 2, 0, 0);
		const Vector3 pr1 = p1 + Vector3(extents.x * 2, 0, 0);
		const Vector3 pr2 = p2 + Vector3(extents.x * 2, 0, 0);
		const Vector3 pr3 = p3 + Vector3(extents.x * 2, 0, 0);
		DrawLine3D(pr0, pr1, thickness, color);
		DrawLine3D(pr1, pr2, thickness, color);
		DrawLine3D(pr2, pr3, thickness, color);
		DrawLine3D(pr0, pr3, thickness, color);

		DrawLine3D(p0, pr0, thickness, color);
		DrawLine3D(p1, pr1, thickness, color);

		DrawLine3D(p2, pr2, thickness, color);
		DrawLine3D(p3, pr3, thickness, color);
	}

	void ShapeRenderer::DrawWireframePlane3D(const Vector3& center, const Vector3& extents, float thickness, const ColorGrad& color)
	{
		const Vector3 p0 = center - Vector3(extents.x * 0.5f, 0.0f, extents.z * 0.5f);
		const Vector3 p1 = p0 + Vector3(extents.x, 0.0f, 0.0f);
		const Vector3 p2 = p1 + Vector3(0.0f, 0.0f, extents.z);
		const Vector3 p3 = p0 + Vector3(0.0f, 0.0f, extents.z);
		DrawLine3D(p0, p1, thickness, color);
		DrawLine3D(p0, p3, thickness, color);
		DrawLine3D(p1, p2, thickness, color);
		DrawLine3D(p3, p2, thickness, color);
	}

	void ShapeRenderer::DrawWireframeCapsule3D(const Vector3& center, float height, float radius, float thickness, const ColorGrad& color)
	{
		const int segments	 = 9; // Number of segments for the circular parts.
		const int stacks	 = 3; // Number of stacks for the hemispheres.
		float	  halfHeight = height * 0.5f;

		// Calculate the top and bottom centers of the capsule.
		Vector3 topCenter	 = center + Vector3(0.0f, halfHeight, 0.0f);
		Vector3 bottomCenter = center - Vector3(0.0f, halfHeight, 0.0f);

		// Draw the cylinder part connecting the top and bottom hemispheres.
		for (int i = 0; i < segments; ++i)
		{
			float angle1 = glm::two_pi<float>() * i / segments;
			float angle2 = glm::two_pi<float>() * (i + 1) / segments;

			Vector3 p1 = Vector3(radius * cos(angle1), 0.0f, radius * sin(angle1));
			Vector3 p2 = Vector3(radius * cos(angle2), 0.0f, radius * sin(angle2));

			// Offset points to match top and bottom circle locations.
			DrawLine3D(topCenter + p1, topCenter + p2, thickness, color);
			DrawLine3D(bottomCenter + p1, bottomCenter + p2, thickness, color);
			DrawLine3D(topCenter + p1, bottomCenter + p1, thickness, color);
		}

		// Draw the top hemisphere.
		for (int i = 0; i < stacks; ++i)
		{
			float theta1 = glm::half_pi<float>() * i / stacks;
			float theta2 = glm::half_pi<float>() * (i + 1) / stacks;

			float r1 = radius * cos(theta1);
			float r2 = radius * cos(theta2);
			float y1 = radius * sin(theta1);
			float y2 = radius * sin(theta2);

			for (int j = 0; j < segments; ++j)
			{
				float angle1 = glm::two_pi<float>() * j / segments;
				float angle2 = glm::two_pi<float>() * (j + 1) / segments;

				Vector3 p1 = Vector3(r1 * cos(angle1), y1, r1 * sin(angle1));
				Vector3 p2 = Vector3(r1 * cos(angle2), y1, r1 * sin(angle2));
				Vector3 p3 = Vector3(r2 * cos(angle1), y2, r2 * sin(angle1));
				Vector3 p4 = Vector3(r2 * cos(angle2), y2, r2 * sin(angle2));

				DrawLine3D(topCenter + p1, topCenter + p2, thickness, color);
				DrawLine3D(topCenter + p1, topCenter + p3, thickness, color);
			}
		}

		// Draw the bottom hemisphere (mirrored top hemisphere).
		for (int i = 0; i < stacks; ++i)
		{
			float theta1 = glm::half_pi<float>() * i / stacks;
			float theta2 = glm::half_pi<float>() * (i + 1) / stacks;

			float r1 = radius * cos(theta1);
			float r2 = radius * cos(theta2);
			float y1 = radius * sin(theta1);
			float y2 = radius * sin(theta2);

			for (int j = 0; j < segments; ++j)
			{
				float angle1 = glm::two_pi<float>() * j / segments;
				float angle2 = glm::two_pi<float>() * (j + 1) / segments;

				Vector3 p1 = Vector3(r1 * cos(angle1), -y1, r1 * sin(angle1));
				Vector3 p2 = Vector3(r1 * cos(angle2), -y1, r1 * sin(angle2));
				Vector3 p3 = Vector3(r2 * cos(angle1), -y2, r2 * sin(angle1));
				Vector3 p4 = Vector3(r2 * cos(angle2), -y2, r2 * sin(angle2));

				DrawLine3D(bottomCenter + p1, bottomCenter + p2, thickness, color);
				DrawLine3D(bottomCenter + p1, bottomCenter + p3, thickness, color);
			}
		}
	}

	void ShapeRenderer::DrawWireframeCylinder3D(const Vector3& center, float height, float radius, float thickness, const ColorGrad& color)
	{
		const Vector3 top = center + Vector3(0.0f, height * 0.5f, 0.0f);
		const Vector3 bot = center - Vector3(0.0f, height * 0.5f, 0.0f);
		DrawWireframeCircle3D(top, radius, thickness, color);
		DrawWireframeCircle3D(bot, radius, thickness, color);

		const int	segments   = 32;
		const float deltaAngle = glm::two_pi<float>() / segments;

		// Draw circles in the XZ plane (latitude).
		for (int i = 0; i < segments; ++i)
		{
			if (i % 4 != 0)
				continue;

			float		  theta0 = i * deltaAngle;
			const Vector3 p0	 = center + Vector3(radius * cos(theta0), height * 0.5f, radius * sin(theta0));
			const Vector3 p1	 = center + Vector3(radius * cos(theta0), -height * 0.5f, radius * sin(theta0));
			DrawLine3D(p0, p1, thickness, color);
		}
	}

	void ShapeRenderer::DrawWireframeCircle3D(const Vector3& center, float radius, float thickness, const ColorGrad& color)
	{
		const int	segments   = 32;
		const float deltaAngle = glm::two_pi<float>() / segments;

		// Draw circles in the XZ plane (latitude).
		for (int i = 0; i < segments; ++i)
		{
			float theta0 = i * deltaAngle;
			float theta1 = (i + 1) * deltaAngle;

			Vector3 p0 = center + Vector3(radius * cos(theta0), 0.0f, radius * sin(theta0));
			Vector3 p1 = center + Vector3(radius * cos(theta1), 0.0f, radius * sin(theta1));

			DrawLine3D(p0, p1, thickness, color);
		}
	}

	void ShapeRenderer::DrawWireframeSphere3D(const Vector3& center, float radius, float thickness, const ColorGrad& color)
	{
		const int	segments   = 32;
		const float deltaAngle = glm::two_pi<float>() / segments;

		// Draw circles in the XZ plane (latitude).
		for (int i = 0; i < segments; ++i)
		{
			float theta0 = i * deltaAngle;
			float theta1 = (i + 1) * deltaAngle;

			Vector3 p0 = center + Vector3(radius * cos(theta0), 0.0f, radius * sin(theta0));
			Vector3 p1 = center + Vector3(radius * cos(theta1), 0.0f, radius * sin(theta1));

			DrawLine3D(p0, p1, thickness, color);
		}

		// Draw circles in the XY plane (longitude).
		for (int i = 0; i < segments; ++i)
		{
			float theta0 = i * deltaAngle;
			float theta1 = (i + 1) * deltaAngle;

			Vector3 p0 = center + Vector3(radius * cos(theta0), radius * sin(theta0), 0.0f);
			Vector3 p1 = center + Vector3(radius * cos(theta1), radius * sin(theta1), 0.0f);

			DrawLine3D(p0, p1, thickness, color);
		}

		// Draw circles in the YZ plane (longitude).
		for (int i = 0; i < segments; ++i)
		{
			float theta0 = i * deltaAngle;
			float theta1 = (i + 1) * deltaAngle;

			Vector3 p0 = center + Vector3(0.0f, radius * cos(theta0), radius * sin(theta0));
			Vector3 p1 = center + Vector3(0.0f, radius * cos(theta1), radius * sin(theta1));

			DrawLine3D(p0, p1, thickness, color);
		}
	}

	void ShapeRenderer::DrawWireframeHemiSphere3D(const Vector3& center, float radius, float thickness, const ColorGrad& color)
	{
		const int	segments   = 32;
		const float deltaAngle = glm::two_pi<float>() / segments;

		// Draw circles in the XZ plane (latitude).
		for (int i = 0; i < segments; ++i)
		{
			float theta0 = i * deltaAngle;
			float theta1 = (i + 1) * deltaAngle;

			Vector3 p0 = center + Vector3(radius * cos(theta0), 0.0f, radius * sin(theta0));
			Vector3 p1 = center + Vector3(radius * cos(theta1), 0.0f, radius * sin(theta1));

			DrawLine3D(p0, p1, thickness, color);
		}

		// Draw circles in the XY plane (longitude).
		for (int i = 0; i < segments; ++i)
		{
			float theta0 = i * deltaAngle;
			float theta1 = (i + 1) * deltaAngle;

			Vector3 p0 = center + Vector3(radius * cos(theta0), radius * sin(theta0), 0.0f);
			Vector3 p1 = center + Vector3(radius * cos(theta1), radius * sin(theta1), 0.0f);

			DrawLine3D(p0, p1, thickness, color);
		}

		// Draw circles in the YZ plane (longitude).
		for (int i = 0; i < segments; ++i)
		{
			float theta0 = i * deltaAngle;
			float theta1 = (i + 1) * deltaAngle;

			Vector3 p0 = center + Vector3(0.0f, radius * cos(theta0), radius * sin(theta0));
			Vector3 p1 = center + Vector3(0.0f, radius * cos(theta1), radius * sin(theta1));

			DrawLine3D(p0, p1, thickness, color);
		}
	}

	void ShapeRenderer::DrawWireCone3D(const Vector3& top, const Vector3& dir, float length, const float radius, float thickness, const ColorGrad& color, bool drawLines)
	{
		const int	segments   = 36; // Number of segments for the base circle.
		const float deltaAngle = glm::two_pi<float>() / segments;

		// Normalize the direction vector.
		Vector3 direction = glm::normalize(dir);

		// Calculate the center of the base circle.
		Vector3 baseCenter = top + direction * length;

		// Create two orthogonal vectors perpendicular to the direction vector.
		Vector3 up		= (fabs(direction.y) < 0.99f) ? Vector3(0.0f, 1.0f, 0.0f) : Vector3(1.0f, 0.0f, 0.0f);
		Vector3 right	= glm::normalize(glm::cross(direction, up));
		Vector3 forward = glm::normalize(glm::cross(right, direction));

		// Draw the base circle.
		Vector<Vector3> circlePoints;
		Vector<Vector3> linePoints;

		for (int i = 0; i < segments; ++i)
		{
			float	angle		  = i * deltaAngle;
			Vector3 pointOnCircle = baseCenter + right * (radius * cos(angle)) + forward * (radius * sin(angle));
			circlePoints.push_back(pointOnCircle);

			if (i % 8 == 0)
				linePoints.push_back(pointOnCircle);

			// Draw a line segment of the circle.
			if (i > 0)
				DrawLine3D(circlePoints[i - 1], circlePoints[i], thickness, color);
		}

		// Connect the last segment to close the circle.
		DrawLine3D(circlePoints.back(), circlePoints[0], thickness, color);

		if (!drawLines)
			return;

		// Draw lines connecting the top point to the base circle.
		for (const Vector3& point : linePoints)
			DrawLine3D(top, point, thickness, color);
	}

} // namespace Lina
