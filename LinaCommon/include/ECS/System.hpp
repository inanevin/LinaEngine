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

/*
Class: System



Timestamp: 12/25/2021 12:38:18 PM
*/

#pragma once

#ifndef System_HPP
#define System_HPP

#include <Data/String.hpp>

namespace Lina
{
    namespace Event
    {
        struct ELevelInstalled;
    }
}

namespace Lina::ECS
{
    class Registry;
    class System
    {

    public:
        System()          = default;
        virtual ~System() = default;

        virtual void Initialize(const String& name);
        virtual void UpdateComponents(float delta) = 0;

        virtual int GetPoolSize() const
        {
            return m_poolSize;
        }

        virtual void SystemActivation(bool active)
        {
            m_isActive = active;
        }

        inline const String& GetName() const
        {
            return m_name;
        }

    protected:

        int         m_poolSize = 0;
        String m_name     = "";
        bool        m_isActive = false;
    };
} // namespace Lina::ECS

#endif
