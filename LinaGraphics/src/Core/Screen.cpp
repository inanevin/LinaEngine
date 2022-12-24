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

#include "Core/Screen.hpp"
#include "Core/Backend.hpp"
#include "PipelineObjects/Swapchain.hpp"
#include "Core/RenderEngine.hpp"
#include "Core/CommonEngine.hpp"

namespace Lina::Graphics
{
    Vector2i Screen::Size()
    {
        return Backend::Get()->GetMainSwapchain().size;
    }

    Vector2 Screen::SizeF()
    {
        const Vector2i size = Size();
        return Vector2(static_cast<float>(size.x), static_cast<float>(size.y));
    }

    Vector2 Screen::DisplayResolutionF()
    {
        return RuntimeInfo::GetDisplayResolutionF();
    }

    Vector2i Screen::DisplayResolution()
    {
        return RuntimeInfo::GetDisplayResolution();
    }

    Vector2i Screen::GetViewportPos()
    {
        auto*       renderEngine = RenderEngine::Get();
        const auto& vp           = renderEngine->GetViewport();
        return Vector2i(static_cast<int>(vp.x), static_cast<int>(vp.y));
    }

    Vector2 Screen::GetViewportPosF()
    {
        const Vector2i pos = GetViewportPos();
        return Vector2(static_cast<float>(pos.x), static_cast<float>(pos.y));
    }

    Vector3 Screen::ScreenToWorldCoordinates(const Vector3& screenPos)
    {
        auto*       renderEngine = RenderEngine::Get();
        const auto& cameraSystem = renderEngine->GetRenderer().GetCameraSystem();

        Vector2 windowSize = SizeF();
        Vector2 windowPos  = GetViewportPosF();
        Vector4 viewport(windowPos.x, windowPos.y, windowSize.x, windowSize.y);
        Vector3 win      = glm::vec3(screenPos.x, windowSize.y - screenPos.y, 1.0f);
        Matrix  pp       = cameraSystem.GetProj();
        Matrix  vv       = cameraSystem.GetView();
        Vector3 camPos   = cameraSystem.GetPos();
        Vector3 worldPos = glm::unProject(win, vv, pp, viewport);
        return Vector3(glm::normalize(worldPos - camPos)) * screenPos.z + camPos;
    }

    Vector3 Screen::ViewportToWorldCoordinates(const Vector3& viewport)
    {
        auto*   renderEngine = Graphics::RenderEngine::Get();
        Vector2 windowSize   = SizeF();
        return ScreenToWorldCoordinates(Vector3(viewport.x * windowSize.x, viewport.y * windowSize.y, viewport.z));
    }

    Vector3 Screen::WorldToScreenCoordinates(const Vector3& world)
    {
        auto*       renderEngine = Graphics::RenderEngine::Get();
        const auto& cameraSystem = renderEngine->GetRenderer().GetCameraSystem();

        Vector2 windowSize = SizeF();
        Vector2 windowPos  = GetViewportPosF();
        Vector4 viewport(windowPos.x, windowPos.y, windowSize.x, windowSize.y);
        Matrix  pp     = cameraSystem.GetProj();
        Matrix  vv     = cameraSystem.GetView();
        Vector3 result = glm::project(world, vv, pp, viewport);
        return Vector3(result.x, windowSize.y - result.y, result.z);
    }

    Vector3 Screen::WorldToViewportCoordinates(const Vector3& world)
    {
        auto*   renderEngine = Graphics::RenderEngine::Get();
        Vector2 windowSize   = SizeF();
        Vector3 screen       = WorldToScreenCoordinates(world);
        return Vector3(screen.x / windowSize.x, screen.y / windowSize.y, screen.z);
    }
    Vector2 Screen::ScreenToViewport(const Vector2& screen)
    {
        const Vector2 size = SizeF();
        return Vector2(screen.x / size.x, screen.y / size.y);
    }

    Vector2 Screen::ViewportToScreen(const Vector2& vp)
    {
        const Vector2 size = SizeF();
        return Vector2(size.x * vp.x, size.y * vp.y);
    }
} // namespace Lina::Graphics
