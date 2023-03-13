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

#ifndef GfxMeshManager_HPP
#define GfxMeshManager_HPP

#include "Graphics/Data/RenderData.hpp"
#include "Data/HashMap.hpp"
#include "Data/Vector.hpp"
#include "Core/StringID.hpp"

namespace Lina
{
	class GfxManager;
	class ResourceManager;
	class Mesh;
	class IGfxBufferResource;
	class Renderer;

	class GfxMeshManager
	{
	public:
		GfxMeshManager(GfxManager* gfxManager);
		~GfxMeshManager() = default;

		void Initialize();
		void Shutdown();
		void MergeMeshes();

		inline IGfxBufferResource* GetGPUVertexBuffer() const
		{
			return m_gpuVtxBuffer;
		}

		inline IGfxBufferResource* GetGPUIndexBuffer() const
		{
			return m_gpuIndexBuffer;
		}

		inline const HashMap<Mesh*, MergedBufferMeshEntry>& GetMergedMeshes() const
		{
			return m_meshEntries;
		}

	private:
		Renderer*							  m_renderer		= nullptr;
		ResourceManager*					  m_resourceManager = nullptr;
		GfxManager*							  m_gfxManager		= nullptr;
		HashMap<Mesh*, MergedBufferMeshEntry> m_meshEntries;
		Vector<StringID>					  m_mergedModelIDs;
		IGfxBufferResource*					  m_gpuVtxBuffer   = nullptr;
		IGfxBufferResource*					  m_gpuIndexBuffer = nullptr;
	};
} // namespace Lina

#endif
