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

#include "Common/StringID.hpp"
#include "Common/Math/Vector.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/Graphics/ResourceUploadQueue.hpp"

namespace LinaGX
{
	class CommandStream;
	class Window;
	class Instance;
} // namespace LinaGX

namespace Lina
{
	class ResourceManagerV2;
	class Shader;
} // namespace Lina

namespace Lina::Editor
{

	class Editor;

	class SurfaceRenderer
	{
	private:
		struct PerFrameData
		{
			LinaGX::CommandStream* gfxStream	   = nullptr;
			LinaGX::CommandStream* copyStream	   = nullptr;
			SemaphoreData		   copySemaphore   = {};
			Buffer				   guiVertexBuffer = {};
			Buffer				   guiIndexBuffer  = {};
		};

		struct DrawRequest
		{
			Shader*	   shader		  = nullptr;
			uint32	   startVertex	  = 0;
			uint32	   startIndex	  = 0;
			uint32	   vertexCount	  = 0;
			uint32	   indexCount	  = 0;
			size_t	   materialOffset = 0;
			Recti	   clip			  = {};
			ResourceID textureID	  = 0;
			size_t	   texturePadding = 0;

			bool _invalid = false;
		};

		struct Draw
		{
			Span<uint8>			guiVertexBuffer		  = {};
			Span<uint8>			guiIndexBuffer		  = {};
			Span<uint8>			materialBuffer		  = {};
			uint32				vertexCounter		  = 0;
			uint32				indexCounter		  = 0;
			size_t				materialBufferCounter = 0;
			Vector<DrawRequest> requests;
		};

	public:
		SurfaceRenderer(Editor* editor, LinaGX::Window* window, const Color& clearColor);
		virtual ~SurfaceRenderer();

		void				   PreTick();
		void				   Tick(float delta);
		void				   SyncRender();
		void				   DropRenderFrame();
		LinaGX::CommandStream* Render(uint32 frameIndex);
		void				   OnWindowSizeChanged(LinaGX::Window* window, const Vector2ui& newSize);
		bool				   CheckVisibility();

		SemaphoreData GetWaitSemaphore(uint32 frameIndex)
		{
			return m_pfd[frameIndex].copySemaphore;
		};

		inline uint8 GetSwapchain() const
		{
			return m_swapchain;
		}

		inline Widget* GetGUIRoot()
		{
			return m_widgetManager.GetRoot();
		}

		inline WidgetManager& GetWidgetManager()
		{
			return m_widgetManager;
		}

		LinaGX::Window* GetWindow() const
		{
			return m_window;
		}

	private:
		void UpdateBuffers(uint32 frameIndex);
		void DrawDefault(LinaVG::DrawBuffer* buf);

	protected:
		Editor*				m_editor			= nullptr;
		ResourceManagerV2*	m_resourceManagerV2 = nullptr;
		LinaGX::Instance*	m_lgx				= nullptr;
		ResourceUploadQueue m_uploadQueue;
		Vector2ui			m_size = Vector2ui::Zero;
		PerFrameData		m_pfd[FRAMES_IN_FLIGHT];
		LinaGX::Window*		m_window	= nullptr;
		uint8				m_swapchain = 0;
		bool				m_isVisible = false;
		RenderPass			m_guiPass	= {};

		LinaVG::Drawer m_lvgDrawer;
		WidgetManager  m_widgetManager;

		Vector<DrawRequest> m_drawRequests;

		Shader* m_guiDefault	= nullptr;
		Shader* m_guiGlitch		= nullptr;
		Shader* m_guiColorWheel = nullptr;
		Shader* m_guiHue		= nullptr;
		Shader* m_guiText		= nullptr;
		Shader* m_guiSDF		= nullptr;

		uint32 m_frameVertexCounter			= 0;
		uint32 m_frameIndexCounter			= 0;
		size_t m_frameMaterialBufferCounter = 0;

		Draw m_cpuDraw	  = {};
		Draw m_renderDraw = {};
	};

} // namespace Lina::Editor
