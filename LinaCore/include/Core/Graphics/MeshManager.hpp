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
	class MeshDefault;
	class ResourceUploadQueue;

	struct MeshBuffer
	{
		Buffer				 vertexBuffer;
		Buffer				 indexBuffer;
		size_t				 startVertex = 0;
		size_t				 startIndex	 = 0;
		Vector<MeshDefault*> meshes;
	};

	class MeshManager
	{
	public:
		MeshManager()  = default;
		~MeshManager() = default;

		void Initialize();
		void Shutdown();

		void AddToUploadQueue(ResourceUploadQueue& queue);
		void BindBuffers(LinaGX::CommandStream* stream, uint32 bufferIndex);
		void AddMesh(MeshDefault* mesh);
		void RemoveMesh(MeshDefault* mesh);

		void Refresh();

	private:
		static constexpr size_t MESH_BUF_SIZE = 1;

		MeshBuffer m_meshBuffers[MESH_BUF_SIZE];
	};
} // namespace Lina
