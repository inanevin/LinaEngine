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

#include "Graphics/Core/Screen.hpp"
#include "Graphics/Core/Backend.hpp"
#include "Graphics/PipelineObjects/Swapchain.hpp"
#include "Core/CommonEngine.hpp"
#include "Graphics/Core/Renderer.hpp"
#include "World/Core/World.hpp"
#include "Graphics/Components/CameraComponent.hpp"

namespace Lina::Graphics
{
    Vector2i Screen::Size() const
    {
        return m_swapchain->size;
    }

    Vector2i Screen::DisplayResolution() const
    {
        return m_displayResolution;
    }

    Vector3 Screen::ScreenToWorldCoordinates(const Vector3& screenPos) const
    {
        auto* world = World::EntityWorld::GetWorld();
        if (!world || world->GetActiveCamera() == nullptr)
            return Vector3::Zero;

        Vector2 windowSize = Size();
        Vector2 windowPos  = m_swapchain->pos;
        Vector4 viewport(windowPos.x, windowPos.y, windowSize.x, windowSize.y);
        Vector3 win = glm::vec3(screenPos.x, windowSize.y - screenPos.y, 1.0f);

        Matrix  pp       = world->GetActiveCamera()->GetProjection();
        Matrix  vv       = world->GetActiveCamera()->GetView();
        Vector3 camPos   = world->GetActiveCamera()->GetEntity()->GetPosition();
        Vector3 worldPos = glm::unProject(win, vv, pp, viewport);
        return Vector3(glm::normalize(worldPos - camPos)) * screenPos.z + camPos;
    }

    Vector3 Screen::ViewportToWorldCoordinates(const Vector3& viewport) const
    {
        Vector2 windowSize = Size();
        return ScreenToWorldCoordinates(Vector3(viewport.x * windowSize.x, viewport.y * windowSize.y, viewport.z));
    }

    Vector3 Screen::WorldToScreenCoordinates(const Vector3& world) const
    {
        auto* w = World::EntityWorld::GetWorld();
        if (!w || w->GetActiveCamera() == nullptr)
            return Vector3::Zero;

        Vector2 windowSize = Size();
        Vector2 windowPos  = m_swapchain->pos;
        Vector4 viewport(windowPos.x, windowPos.y, windowSize.x, windowSize.y);
        Matrix  pp     = w->GetActiveCamera()->GetProjection();
        Matrix  vv     = w->GetActiveCamera()->GetView();
        Vector3 result = glm::project(world, vv, pp, viewport);
        return Vector3(result.x, windowSize.y - result.y, result.z);
    }

    Vector3 Screen::WorldToViewportCoordinates(const Vector3& world) const
    {
        Vector2 windowSize = Size();
        Vector3 screen     = WorldToScreenCoordinates(world);
        return Vector3(screen.x / windowSize.x, screen.y / windowSize.y, screen.z);
    }
    Vector2 Screen::ScreenToViewport(const Vector2& screen) const
    {
        const Vector2 size = Size();
        return Vector2(screen.x / size.x, screen.y / size.y);
    }

    Vector2 Screen::ViewportToScreen(const Vector2& vp) const
    {
        const Vector2 size = Size();
        return Vector2(size.x * vp.x, size.y * vp.y);
    }
} // namespace Lina::Graphics
