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

#include "Core/Graphics/Pipeline/Buffer.hpp"

namespace LinaGX
{
	class CommandStream;
}
namespace Lina
{
	class Model;
	struct Mesh;
	class ResourceUploadQueue;

	struct MeshBuffer
	{
		Buffer		  vertexBuffer;
		Buffer		  indexBuffer;
		size_t		  startVertex = 0;
		size_t		  startIndex  = 0;
		Vector<Mesh*> meshes;
	};

	class MeshManager
	{
	public:
		MeshManager()  = default;
		~MeshManager() = default;

		void Initialize();
		void Shutdown();

		void AddToUploadQueue(ResourceUploadQueue& queue);
		void BindStatic(LinaGX::CommandStream* stream);
		void BindSkinned(LinaGX::CommandStream* stream);

		void AddMesh(Mesh* mesh);
		void RemoveMesh(Mesh* mesh);

		void Refresh();

		inline Buffer& GetVtxBufferStatic()
		{
			return m_bufferStatic.vertexBuffer;
		}

		inline Buffer& GetIdxBufferStatic()
		{
			return m_bufferStatic.indexBuffer;
		}

		inline Buffer& GetVtxBufferSkinned()
		{
			return m_bufferSkinned.vertexBuffer;
		}

		inline Buffer& GetIdxBufferSkinned()
		{
			return m_bufferSkinned.indexBuffer;
		}

	private:
		void CreateBuffer(MeshBuffer& buf, size_t vtxSz, size_t idxSz);
		void DestroyBuffer(MeshBuffer& buf);

	private:
		MeshBuffer m_bufferSkinned;
		MeshBuffer m_bufferStatic;
	};
} // namespace Lina
