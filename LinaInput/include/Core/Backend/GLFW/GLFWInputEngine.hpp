/*
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: InputEngine

Acts as a wrapper for input device functionality and also manages any additional functionality.

Timestamp: 4/14/2019 7:46:20 PM
*/

#pragma once

#ifndef GLFWInputEngine_HPP
#define GLFWInputEngine_HPP

#include "Core/CommonInput.hpp"
#include "ECS/Systems/FreeLookSystem.hpp"
#include "ECS/SystemList.hpp"
#include "Core/InputAxis.hpp"
#include "Core/InputMappings.hpp"
#include "Math/Vector.hpp"

#include <map>

namespace Lina
{
    class Engine;

    namespace Event
    {
        struct EWindowContextCreated;
        struct EMouseScrollCallback;
    } // namespace Event

} // namespace Lina

namespace Lina::Input
{
    class GLFWInputEngine
    {

    public:
        static GLFWInputEngine* Get()
        {
            return s_inputEngine;
        }
        bool           GetKey(int keyCode);
        bool           GetKeyDown(int keyCode);
        bool           GetKeyUp(int keyCode);
        bool           GetMouseButton(int index);
        bool           GetMouseButtonDown(int index);
        bool           GetMouseButtonUp(int index);
        void           SetCursorMode(CursorMode mode);
        void           SetMousePosition(const Vector2& v) const;
        Vector2        GetMousePosition();
        Vector2        GetRawMouseAxis();
        Vector2        GetMouseAxis();
        inline Vector2 GetMouseScroll()
        {
            return m_currentMouseScroll;
        }

        /// <summary>
        /// Any system added to the input pipeline will get updated during polling inputs.
        /// </summary>
        void AddToInputPipeline(ECS::System& system);

        inline CursorMode GetCursorMode()
        {
            return m_cursorMode;
        }
        inline float GetHorizontalAxisValue()
        {
            return m_horizontalAxis.GetValue();
        }
        inline float GetVerticalAxisValue()
        {
            return m_verticalAxis.GetValue();
        }
        inline const ECS::SystemList& GetPipeline()
        {
            return m_inputPipeline;
        }

    private:
        friend class Engine;
        GLFWInputEngine()  = default;
        ~GLFWInputEngine() = default;
        void Initialize();
        void Tick();
        void Shutdown();
        void OnWindowContextCreated(const Event::EWindowContextCreated& e);
        void OnMouseScrollCallback(const Event::EMouseScrollCallback& e);

    private:
        friend class Engine;
        static GLFWInputEngine* s_inputEngine;
        int                     m_keyStatesDown[NUM_KEY_STATES];
        int                     m_keyStatesUp[NUM_KEY_STATES];
        int                     m_mouseStatesDown[NUM_MOUSE_STATES];
        int                     m_mouseStatesUp[NUM_MOUSE_STATES];
        std::map<int, int>      m_keyDownNewStateMap;
        std::map<int, int>      m_keyUpNewStateMap;
        std::map<int, int>      m_mouseDownNewStateMap;
        std::map<int, int>      m_mouseUpNewStateMap;
        InputAxis               m_horizontalAxis;
        InputAxis               m_verticalAxis;
        CursorMode              m_cursorMode = CursorMode::Visible;
        ECS::FreeLookSystem     m_freeLookSystem;
        ECS::SystemList         m_inputPipeline;
        Vector2                 m_currentMouseScroll = Vector2::Zero;
    };
} // namespace Lina::Input

#endif
