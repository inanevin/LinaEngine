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

#include "Input/Core/InputAxis.hpp"

#include "EventSystem/EventSystem.hpp"
#include "EventSystem/InputEvents.hpp"
#include "Input/Core/InputCommon.hpp"

namespace Lina::Input
{
    void InputAxis::BindAxis(int positiveKey, int negativeKey)
    {
        m_positiveKey = positiveKey;
        m_negativeKey = negativeKey;
        Event::EventSystem::Get()->Connect<Event::EKeyCallback, &InputAxis::OnKey>(this);
    }

    void InputAxis::OnKey(const Event::EKeyCallback& key)
    {
        if (key.action == static_cast<int>(InputAction::Pressed))
        {
            if (key.key == m_positiveKey)
                m_positiveValue = 1.0f;
            else if (key.key == m_negativeKey)
                m_negativeValue = -1.0f;
        }
        else if (key.action == static_cast<int>(InputAction::Released))
        {
            if (key.key == m_positiveKey)
                m_positiveValue = 0.0f;
            else if (key.key == m_negativeKey)
                m_negativeValue = 0.0f;
        }
    }
} // namespace Lina::Input
