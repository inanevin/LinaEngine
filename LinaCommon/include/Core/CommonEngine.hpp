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

#ifndef CommonEngine_HPP
#define CommonEngine_HPP

#include "Data/Vector.hpp"
#include "Data/HashMap.hpp"
#include "Math/Vector.hpp"
#include "Utility/StringId.hpp"
#include "Data/String.hpp"

namespace Lina
{
    namespace Editor
    {
        class Editor;
    }

    namespace Graphics
    {
        class Swapchain;
        class Win32Window;
    } // namespace Graphics

    class DefaultResources
    {
    public:
        static inline const HashMap<TypeID, Vector<String>>& GetEngineResources()
        {
            return s_engineResources;
        }

        static bool IsEngineResource(TypeID tid, StringID sid);

    private:
        friend class Engine;

        static HashMap<TypeID, Vector<String>> s_engineResources;
    };

    class RuntimeInfo
    {
    public:
        static inline bool GetIsInPlayMode()
        {
            return s_isInPlayMode;
        }

        static double GetElapsedTime();

        static inline float GetDeltaTime()
        {
            return s_deltaTime;
        }

        static inline float GetSmoothDeltaTime()
        {
            return s_smoothDeltaTime;
        }

        static Vector2i GetScreenSize()
        {
            return s_screenSize;
        }

        static Vector2 GetScreenSizeF()
        {
            return Vector2(static_cast<float>(s_screenSize.x), static_cast<float>(s_screenSize.y));
        }

        static Vector2i GetDisplayResolution()
        {
            return s_displayResolution;
        }

        static Vector2 GetDisplayResolutionF()
        {
            return Vector2(static_cast<float>(s_displayResolution.x), static_cast<float>(s_displayResolution.y));
        }

        static inline float GetContentScaleWidth()
        {
            return s_contentScaleWidth;
        }

        static inline float GetContentScaleHeight()
        {
            return s_contentScaleHeight;
        }

    private:
        friend class Engine;
        friend class Editor::Editor;
        friend class Graphics::Win32Window;
        friend class Graphics::Swapchain;

        static double   s_startTime;
        static bool     s_isInPlayMode;
        static bool     s_paused;
        static bool     s_shouldSkipFrame;
        static float    s_deltaTime;
        static float    s_smoothDeltaTime;
        static float    s_contentScaleWidth;
        static float    s_contentScaleHeight;
        static Vector2i s_screenSize;
        static Vector2i s_displayResolution;
    };

    extern TypeID g_levelTypeID;

#define LINA_EDITOR_CAMERA_NAME "lina_entityreserved_editorcam"
} // namespace Lina

#endif
