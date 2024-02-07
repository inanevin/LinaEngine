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

#include "Common/Data/Vector.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Data/HashMap.hpp"
#include "Common/Event/Event.hpp"
#include "Common/Math/Rect.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Graphics/CommonGraphics.hpp"

namespace LinaGX
{
	class Window;
	class CommandStream;
	class Instance;
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
	class Buffer;
	class Shader;

	class GUIBackend : public LinaVG::Backend::BaseBackend
	{

	public:
		struct RenderData
		{
			Vector2ui			   size			   = Vector2ui::Zero;
			LinaGX::CommandStream* gfxStream	   = nullptr;
			LinaGX::CommandStream* copyStream	   = nullptr;
			SemaphoreData*		   copySemaphore   = nullptr;
			uint16				   descriptorSet2  = 0;
			GfxShaderVariantType   variantPassType = GfxShaderVariantType::RenderTarget;
		};

		struct Buffers
		{
			Buffer* vertexBuffer   = nullptr;
			Buffer* indexBuffer	   = nullptr;
			Buffer* materialBuffer = nullptr;
			uint32	indexCounter   = 0;
			uint32	vertexCounter  = 0;
		};

	private:
		struct DrawRequest
		{
			uint32		   firstIndex	= 0;
			uint32		   vertexOffset = 0;
			uint32		   indexCount	= 0;
			Rectui		   clip			= {};
			GPUMaterialGUI materialData;
			GPUConstantGUI constants;
		};

		struct DrawData
		{
			Vector<DrawRequest> drawRequests;
		};

		struct FontTexture
		{
			Texture* texture = nullptr;
			uint8*	 pixels	 = nullptr;
			int		 width	 = 0;
			int		 height	 = 0;
		};

	public:
		GUIBackend(GfxManager* man);
		~GUIBackend() = default;

		// Inherited via BaseBackend
		virtual bool Initialize() override
		{
			return true;
		}
		virtual void				  Terminate() override;
		virtual void				  StartFrame(int threadCount) override;
		virtual void				  DrawGradient(LinaVG::GradientDrawBuffer* buf, int thread) override;
		virtual void				  DrawTextured(LinaVG::TextureDrawBuffer* buf, int thread) override;
		virtual void				  DrawDefault(LinaVG::DrawBuffer* buf, int thread) override;
		virtual void				  DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf, int thread) override;
		virtual void				  DrawSDFText(LinaVG::SDFTextDrawBuffer* buf, int thread) override;
		virtual void				  EndFrame() override{};
		virtual void				  BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data) override;
		virtual void				  BufferEnded() override;
		virtual void				  BindFontTexture(LinaVG::BackendHandle texture) override;
		virtual void				  SaveAPIState() override{};
		virtual void				  RestoreAPIState() override{};
		virtual LinaVG::BackendHandle CreateFontTexture(int width, int height) override;

		void			 Prepare(int threadIndex, const Buffers& bufferData, const RenderData& renderData);
		Vector<Texture*> GetFontTextures();
		void			 Render(int threadIndex);

	private:
		DrawRequest& AddDrawRequest(LinaVG::DrawBuffer* buf, int threadIndex);
		Matrix4		 GetProjectionFromSize(const Vector2ui& size);

	private:
		StringID		   m_boundFontTexture = 0;
		GfxManager*		   m_gfxManager		  = nullptr;
		ResourceManager*   m_resourceManager  = nullptr;
		Vector<Buffers>	   m_buffers;
		Vector<RenderData> m_renderData;
		Vector<DrawData>   m_drawData;

		LinaGX::Instance*	m_lgx	 = nullptr;
		Shader*				m_shader = nullptr;
		Vector<FontTexture> m_fontTextures;
	};
} // namespace Lina

#endif
