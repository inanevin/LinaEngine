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

#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"

namespace LinaGX
{
	class Instance;
	class CommandStream;
} // namespace LinaGX

namespace Lina
{
	class GfxManager;
	class GUIBackend;
	class Material;

	class GUIRenderer
	{

	private:
		static constexpr int32 MAX_GUI_MATERIALS = 50;

		struct PerFrameData
		{
			LinaGX::CommandStream* copyStream		 = nullptr;
			SemaphoreData		   copySemaphore	 = {};
			Buffer				   guiVertexBuffer	 = {};
			Buffer				   guiIndexBuffer	 = {};
			Buffer				   guiMaterialBuffer = {};
			Material*			   materials		 = nullptr;
		};

	public:
		GUIRenderer()  = default;
		~GUIRenderer() = default;

		void Create(GfxManager* gfxManager);
		void Render(LinaGX::CommandStream* stream, uint32 frameIndex, uint32 threadIndex);
		void Destroy();

		inline const SemaphoreData& GetCopySemaphoreData(uint32 frameIndex) const
		{
			return m_pfd[frameIndex].copySemaphore;
		}

	private:
		GUIBackend*		  m_guiBackend = nullptr;
		PerFrameData	  m_pfd[FRAMES_IN_FLIGHT];
		GfxManager*		  m_gfxManager = nullptr;
		LinaGX::Instance* m_lgx		   = nullptr;
	};

} // namespace Lina
