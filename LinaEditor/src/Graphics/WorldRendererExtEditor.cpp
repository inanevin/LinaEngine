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

#include "Editor/Graphics/WorldRendererExtEditor.hpp"
#include "Editor/Editor.hpp"
#include "Common/System/System.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Components/CameraComponent.hpp"

namespace Lina::Editor
{
	WorldRendererExtEditor::WorldRendererExtEditor()
	{
		m_gfxManager  = Editor::Get()->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		m_rm		  = Editor::Get()->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_shaderLines = m_rm->GetResource<Shader>("Resources/Editor/Shaders/Lines.linashader"_hs);
		m_lgx		  = m_gfxManager->GetLGX();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto&		 data = m_pfd[i];
			const String istr = TO_STRING(i);
			data.vertexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_VertexBuffer, 100 * sizeof(LineVertex), "EditorExt: Vertex");
			data.indexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_IndexBuffer, 100 * sizeof(uint16), "EditorExt: Index");
		}
	}

	WorldRendererExtEditor::~WorldRendererExtEditor()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto&		 data = m_pfd[i];
			const String istr = TO_STRING(i);
			data.vertexBuffer.Destroy();
			data.indexBuffer.Destroy();
		}
	}

	void WorldRendererExtEditor::Tick(float delta)
	{
	}

	namespace
	{
		void DrawLine(BufferBatch& buffers, const Vector3& p1, const Vector3& p2, const Color& startColor = Color::White, const Color& endColor = Color::White)
		{
			LineVertex v0, v1, v2, v3;

			v0.position		= p1;
			v0.nextPosition = p2;
			v0.uv			= Vector2(0, 1);
			v0.direction	= 1.0f;
			v0.color		= startColor;

			v1.position		= p1;
			v1.nextPosition = p2;
			v1.uv			= Vector2(0, 0);
			v1.direction	= -1.0f;
			v1.color		= startColor;

			v2.position		= p2;
			v2.nextPosition = p1;
			v2.uv			= Vector2(1, 1);
			v2.direction	= -1.0f;
			v2.color		= endColor;

			v3.position		= p2;
			v3.nextPosition = p1;
			v3.uv			= Vector2(1, 0);
			v3.direction	= 1.0f;
			v3.color		= endColor;

			Vector<LineVertex> lines;
			lines.push_back(v0);
			lines.push_back(v1);
			lines.push_back(v2);
			lines.push_back(v3);

			Vector<uint16> indices;
			indices.push_back(buffers.vtxCount + 0);
			indices.push_back(buffers.vtxCount + 2);
			indices.push_back(buffers.vtxCount + 1);

			indices.push_back(buffers.vtxCount + 1);
			indices.push_back(buffers.vtxCount + 2);
			indices.push_back(buffers.vtxCount + 3);

			buffers.vtxBuffer->BufferData(sizeof(LineVertex) * buffers.vtxCount, (uint8*)lines.data(), sizeof(LineVertex) * lines.size());
			buffers.idxBuffer->BufferData(sizeof(uint16) * buffers.idxCount, (uint8*)indices.data(), sizeof(uint16) * indices.size());
			buffers.vtxCount += 4;
			buffers.idxCount += 6;
		}
	} // namespace

	bool WorldRendererExtEditor::CopyBuffers(uint32 frameIndex, LinaGX::CommandStream* stream)
	{
		auto&		  data		= m_pfd[frameIndex];
		const Vector3 lineStart = Vector3(5, 0, -12);
		const Vector3 lineEnd	= Vector3(15, 10, 0);

		BufferBatch b;
		b.vtxBuffer = &data.vertexBuffer;
		b.idxBuffer = &data.indexBuffer;

		DrawLine(b, Vector3(-50, 5, 50), Vector3(50, 5, 50));
		DrawLine(b, Vector3(-50, -5, 50), Vector3(50, -5, 50));
		DrawLine(b, Vector3(-50, 5, 50), Vector3(-50, -5, 50));
		DrawLine(b, Vector3(50, 5, 50), Vector3(50, -5, 50));

		DrawLine(b, Vector3(-50, 5, -50), Vector3(50, 5, -50));
		DrawLine(b, Vector3(-50, -5, -50), Vector3(50, -5, -50));
		DrawLine(b, Vector3(-50, 5, -50), Vector3(-50, -5, -50));
		DrawLine(b, Vector3(50, 5, -50), Vector3(50, -5, -50));

		DrawLine(b, Vector3(-50, 5, 50), Vector3(-50, 5, -50));
		DrawLine(b, Vector3(-50, -5, 50), Vector3(-50, -5, -50));
		DrawLine(b, Vector3(50, 5, 50), Vector3(50, 5, -50));
		DrawLine(b, Vector3(50, -5, 50), Vector3(50, -5, -50));

		uint8 copyExists = data.vertexBuffer.Copy(stream);
		copyExists |= data.indexBuffer.Copy(stream);

		return copyExists == 1;
	}

	void WorldRendererExtEditor::RenderForward(uint32 frameIndex, LinaGX::CommandStream* stream)
	{
		auto& data = m_pfd[frameIndex];

		data.vertexBuffer.BindVertex(stream, sizeof(LineVertex));
		data.indexBuffer.BindIndex(stream, LinaGX::IndexType::Uint16);

		m_shaderLines->Bind(stream, m_shaderLines->GetGPUHandle());
		LinaGX::CMDDrawIndexedInstanced* draw = stream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
		draw->indexCountPerInstance			  = 72;
		draw->instanceCount					  = 1;
	}

} // namespace Lina::Editor
