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

#ifndef Screen_HPP
#define Screen_HPP

#include "Math/Vector.hpp"

namespace Lina::Graphics
{
    class Renderer;
    class CameraSystem;
    class RenderEngine;
    class Swapchain;
    class WindowManager;

    class Screen
    {
    public:
        /// <summary>
        /// Returns the total size in screen space.
        /// NOTE: This is not the monitor size.
        /// NOTE: This may not be the same as application resolution if running on windowed mode.
        /// </summary>
        /// <returns></returns>
        Vector2i Size() const;

        /// <summary>
        /// Returns the display resolution of the primary monitor.
        /// </summary>
        /// <returns></returns>
        Vector2i DisplayResolution() const;

        /// <summary>
        /// Returns viewport position, defaults to 0-0 unless explicitly stated.
        /// </summary>
        /// <returns></returns>
        Vector2i GetViewportPos() const;

        /// <summary>
        /// Converts given screen-space coordinates to world-space coordinates.
        /// Z element is the distance from camera.
        /// (0,0) top-left, (screenSizeX, screenSizeY) bottom-right
        /// </summary>
        /// <param name="screenPos"></param>
        /// <returns></returns>
        Vector3 ScreenToWorldCoordinates(const Vector3& screenPos) const;

        /// <summary>
        /// Converts given viewport-space coordinates to world-space coordinates.
        /// (0,0) top-left, (1,1) bottom-right
        /// </summary>
        /// <param name="viewport"></param>
        /// <returns></returns>
        Vector3 ViewportToWorldCoordinates(const Vector3& viewport) const;

        /// <summary>
        /// Converts the given world-space coordinates to screen-space coordinates.
        /// (0,0) top-left, (screenSizeX, screenSizeY) bottom-right
        /// </summary>
        /// <param name="world"></param>
        /// <returns></returns>
        Vector3 WorldToScreenCoordinates(const Vector3& world) const;

        /// <summary>
        /// Converts the given world-space coordinates to viewport coordinates.
        /// (0,0) top-left, (1,1) bottom-right
        /// </summary>
        /// <param name="world"></param>
        /// <returns></returns>
        Vector3 WorldToViewportCoordinates(const Vector3& world) const;

        /// <summary>
        /// Converts from given screen coordinates to viewport coordinates.
        /// (0,0) top-left, (1,1) bottom-right
        /// </summary>
        /// <returns></returns>
        Vector2 ScreenToViewport(const Vector2& screen) const;

        /// <summary>
        /// Converts from given viewport coordinates to screen coordinates.
        /// (0,0) top-left, (1,1) bottom-right
        /// </summary>
        /// <returns></returns>
        Vector2 ViewportToScreen(const Vector2& vp) const;

        /// <summary>
        /// Returns platform DPI scale
        /// </summary>
        inline Vector2 GetContentScale() const
        {
            return m_contentScale;
        }

    private:
        friend class RenderEngine;
        friend class WindowManager;

        inline void Initialize(Renderer* rend, Swapchain* swp)
        {
            m_renderer  = rend;
            m_swapchain = swp;
        }

    private:
        Vector2i   m_displayResolution = Vector2i::Zero;
        Vector2    m_contentScale      = Vector2::Zero;
        Swapchain* m_swapchain         = nullptr;
        Renderer*  m_renderer          = nullptr;
    };
} // namespace Lina::Graphics

#endif
