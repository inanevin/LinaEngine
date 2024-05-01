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

#include "Editor/Widgets/Panel/PanelWorld.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/System.hpp"
#include "Common/Math/Math.hpp"
#include "Core/World/WorldManager.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"

namespace Lina::Editor
{
	void PanelWorld::Construct()
	{
		m_wm	 = m_system->CastSubsystem<WorldManager>(SubsystemType::WorldManager);
		m_gfxMan = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
	}

	void PanelWorld::Tick(float delta)
	{
		return;
		m_world = m_wm->GetMainWorld();
		if (!m_world)
			return;

		m_worldRenderer = m_wm->GetWorldRenderer(m_world);

		const Vector2ui size = Vector2ui(static_cast<uint32>(Math::CeilToInt(GetSizeX())), static_cast<uint32>(Math::CeilToInt(GetSizeY())));
		m_wm->ResizeWorldTexture(m_world, size);
	}

	void PanelWorld::Draw(int32 threadIndex)
	{
		return;

		LinaVG::StyleOptions opts;
		opts.color = Theme::GetDef().background1.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);

		if (!m_world)
			return;

		// LinaVG::DrawImage(threadIndex, m_worldRenderer->GetTexture(m_gfxMan->GetLGX()->GetCurrentFrameIndex()), m_rect.GetCenter().AsLVG(), m_rect.size.AsLVG(), {1, 1, 1, 1}, 0.0f, m_drawOrder);
	}

} // namespace Lina::Editor
