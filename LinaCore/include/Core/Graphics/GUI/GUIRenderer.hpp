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
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include <LinaVG/LinaVG.hpp>

namespace LinaGX
{
	class Instance;
	class CommandStream;
	class Window;
} // namespace LinaGX

namespace Lina
{
	class Material;
	class Shader;
	class RenderPass;
	class TextureSampler;
	class ResourceUploadQueue;
	class GUIBackend;

	class GUIRenderer
	{

	private:
		static constexpr size_t MAX_GUI_MATERIALS = 50;

		struct PerFrameData
		{
			Buffer guiVertexBuffer = {};
			Buffer guiIndexBuffer  = {};
			uint32 vertexCounter   = 0;
			uint32 indexCounter	   = 0;
		};

	public:
		struct DrawRequest
		{
			uint32		   firstIndex	= 0;
			uint32		   vertexOffset = 0;
			uint32		   indexCount	= 0;
			GPUMaterialGUI materialData;
		};

		GUIRenderer()  = default;
		~GUIRenderer() = default;

		void					   Create(GUIBackend* guiBackend, ResourceManagerV2* resourceManager, TextureSampler* defaultSampler, TextureSampler* textSampler, LinaGX::Window* window);
		void					   PreTick();
		void					   Tick(float delta, const Vector2ui& size);
		void					   DrawDefault(LinaVG::DrawBuffer* buf);
		void					   DrawGradient(LinaVG::GradientDrawBuffer* buf);
		void					   DrawTextured(LinaVG::TextureDrawBuffer* buf);
		void					   DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf);
		void					   DrawSDFText(LinaVG::SDFTextDrawBuffer* buf);
		const Vector<DrawRequest>& FlushGUI(uint32 frameIndex, size_t indirectBufferOffset, const Vector2ui& size);
		bool					   CopyVertexIndex(uint32 frameIndex, LinaGX::CommandStream* copyStream);
		void					   AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue);
		void					   Render(LinaGX::CommandStream* stream, const Buffer& indirectBuffer, uint32 frameIndex, const Vector2ui& size);
		void					   Destroy();

		inline Widget* GetGUIRoot() const
		{
			return m_widgetManager.GetRoot();
		}

		inline WidgetManager& GetWidgetManager()
		{
			return m_widgetManager;
		}

	private:
		DrawRequest& AddDrawRequest(LinaVG::DrawBuffer* buf);

	private:
		GUIBackend*			m_guiBackend;
		ResourceManagerV2*	m_resourceManager;
		PerFrameData		m_pfd[FRAMES_IN_FLIGHT];
		LinaGX::Instance*	m_lgx	 = nullptr;
		LinaGX::Window*		m_window = nullptr;
		LinaVG::Drawer		m_lvg;
		WidgetManager		m_widgetManager;
		Vector<DrawRequest> m_drawRequests;
		TextureSampler*		m_defaultGUISampler	   = nullptr;
		TextureSampler*		m_textGUISampler	   = nullptr;
		uint32				m_totalIndices		   = 0;
		size_t				m_indirectBufferOffset = 0;
	};

} // namespace Lina
