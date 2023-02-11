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

#pragma once

#ifndef DrawPass_HPP
#define DrawPass_HPP

#include "Data/Mutex.hpp"
#include "Data/Vector.hpp"
#include "Data/HashMap.hpp"
#include "Graphics/Data/RenderData.hpp"

namespace Lina
{
	class View;
	class Executor;
	class Buffer;
	class CommandBuffer;
	class GPUStorage;

	class DrawPass
	{
	public:
		DrawPass(DrawPassMask mask, float distance, GPUStorage* storage, Executor* exec) : m_passMask(mask), m_drawDistance(distance), m_gpuStorage(storage), m_exec(exec){};

		DrawPass(const DrawPass& other)
		{
			LOCK_GUARD(m_mtx);
			m_renderables  = other.m_renderables;
			m_batches	   = other.m_batches;
			m_passMask	   = other.m_passMask;
			m_drawDistance = other.m_drawDistance;
			m_exec		   = other.m_exec;
			m_gpuStorage   = other.m_gpuStorage;
		}

		DrawPass& operator=(const DrawPass& other)
		{
			LOCK_GUARD(m_mtx);
			m_renderables  = other.m_renderables;
			m_batches	   = other.m_batches;
			m_passMask	   = other.m_passMask;
			m_drawDistance = other.m_drawDistance;
			m_exec		   = other.m_exec;
			m_gpuStorage   = other.m_gpuStorage;
			return *this;
		}

		virtual ~DrawPass() = default;

		void PrepareRenderData(Vector<RenderableData>& drawList, const View& view);
		void UpdateViewData(Buffer& viewDataBuffer, const View& view);
		void RecordDrawCommands(const CommandBuffer& cmd, const HashMap<Mesh*, MergedBufferMeshEntry>& mergedMeshes, Buffer& indirectBuffer, uint32 imageIndex);

	private:
		void ExtractPassRenderables(const Vector<RenderableData>& drawList, const View& view);
		void BatchPassRenderables();

		Vector<RenderableData>& GetVisibleRenderables()
		{
			return m_renderables;
		}

	private:
		int32 FindInBatches(const MeshMaterialPair& pair);

	private:
		GPUStorage*			   m_gpuStorage = nullptr;
		Executor*			   m_exec		= nullptr;
		Mutex				   m_mtx;
		Vector<RenderableData> m_renderables;
		Vector<InstancedBatch> m_batches;
		DrawPassMask		   m_passMask	  = DrawPassMask::Opaque;
		float				   m_drawDistance = 0.0f;
	};
} // namespace Lina

#endif
