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

#ifndef GUIBackend_HPP
#define GUIBackend_HPP

#include "Data/Vector.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Data/HashMap.hpp"
#include "Event/Event.hpp"
#include "Math/Rect.hpp"

namespace LinaGX
{
	class Window;
} // namespace LinaGX

namespace LinaVG
{
	struct DrawBuffer;
}

namespace Lina
{
	class GfxManager;
	class ResourceManager;
	class Texture;
	class GPUBuffer;

	class GUIBackend : public LinaVG::Backend::BaseBackend
	{
	public:
		struct DrawRequest
		{
			uint32 firstIndex	= 0;
			uint32 vertexOffset = 0;
			uint32 indexCount	= 0;
			Rectui clip			= {};
		};

		struct GUIRenderData
		{
			Vector2ui			   size			= Vector2ui::Zero;
			LinaGX::CommandStream* gfxStream	= nullptr;
			LinaGX::CommandStream* copyStream	= nullptr;
			GPUBuffer*			   vertexBuffer = nullptr;
			GPUBuffer*			   indexBuffer	= nullptr;
			Vector<DrawRequest>	   drawRequests;
			uint32				   indexCounter	 = 0;
			uint32				   vertexCounter = 0;
		};

	public:
		GUIBackend(GfxManager* man);
		~GUIBackend() = default;

		// Inherited via BaseBackend
		virtual bool				  Initialize() override;
		virtual void				  Terminate() override;
		virtual void				  StartFrame(int threadCount) override;
		virtual void				  DrawGradient(LinaVG::GradientDrawBuffer* buf, int thread) override;
		virtual void				  DrawTextured(LinaVG::TextureDrawBuffer* buf, int thread) override;
		virtual void				  DrawDefault(LinaVG::DrawBuffer* buf, int thread) override;
		virtual void				  DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf, int thread) override;
		virtual void				  DrawSDFText(LinaVG::SDFTextDrawBuffer* buf, int thread) override;
		virtual void				  EndFrame() override;
		virtual void				  BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data) override;
		virtual void				  BindFontTexture(LinaVG::BackendHandle texture) override;
		virtual void				  SaveAPIState(){};
		virtual void				  RestoreAPIState(){};
		virtual LinaVG::BackendHandle CreateFontTexture(int width, int height) override;

		void Prepare(int threadID, const GUIRenderData& data);
		void Render(int threadID);
		void BindTextures();

	private:
		DrawRequest& AddDrawRequest(LinaVG::DrawBuffer* buf, GUIRenderData& data);

	private:
		HashMap<Texture*, bool>		m_textureDirtyStatus;
		StringID					m_boundFontTexture	 = 0;
		uint32						m_fontTextureCounter = 0;
		HashMap<StringID, Texture*> m_fontTextures;
		HashMap<StringID, Texture*> m_renderReadyFontTextures;
		GfxManager*					m_gfxManager	  = nullptr;
		ResourceManager*			m_resourceManager = nullptr;
		Vector<GUIRenderData>		m_guiRenderData;
	};
} // namespace Lina

#endif