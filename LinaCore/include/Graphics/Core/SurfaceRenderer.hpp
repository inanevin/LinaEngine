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

#ifndef SurfaceRenderer_HPP
#define SurfaceRenderer_HPP

#include "Event/ISystemEventListener.hpp"
#include "Core/StringID.hpp"
#include "Math/Vector.hpp"

namespace Lina
{
	class IGUIDrawer;
	class GUIRenderer;
	class GfxManager;

	class SurfaceRenderer : public ISystemEventListener
	{
	public:
		SurfaceRenderer(GfxManager* man, StringID sid, const Vector2ui& initialSize);
		virtual ~SurfaceRenderer();

		void		 Render(int surfaceRendererIndex, uint32 frameIndex);
		void		 Present();
		virtual void OnSystemEvent(SystemEvent eventType, const Event& ev) override;
		void		 SetGUIDrawer(IGUIDrawer* rend);

		virtual Bitmask32 GetSystemEventMask() override
		{
			return EVS_WindowResized;
		}

		inline int GetSurfaceRendererIndex() const
		{
			return m_surfaceRendererIndex;
		}

		inline IGUIDrawer* GetGUIDrawer() const
		{
			return m_guiDrawer;
		}

		inline StringID GetSID() const
		{
			return m_sid;
		}

	protected:
		static int s_surfaceRendererCount;
		int		   m_surfaceRendererIndex = 0;

		StringID	 m_sid		   = 0;
		GUIRenderer* m_guiRenderer = nullptr;
		GfxManager*	 m_gfxManager  = nullptr;
		IGUIDrawer*	 m_guiDrawer   = nullptr;
	};

} // namespace Lina

#endif
