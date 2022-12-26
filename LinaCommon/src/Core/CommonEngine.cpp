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

#include "Core/CommonEngine.hpp"
#include "Core/Time.hpp"

namespace Lina
{
    HashMap<TypeID, Vector<String>> DefaultResources::s_engineResources;
    double                          RuntimeInfo::s_startTime          = 0.0;
    bool                            RuntimeInfo::s_isInPlayMode       = false;
    bool                            RuntimeInfo::s_paused             = false;
    bool                            RuntimeInfo::s_shouldSkipFrame    = false;
    float                           RuntimeInfo::s_smoothDeltaTime    = 0.0f;
    float                           RuntimeInfo::s_deltaTime          = 0.0f;

    bool DefaultResources::IsEngineResource(TypeID tid, StringID sid)
    {
        const auto& it = s_engineResources.find(tid);
        if (it != s_engineResources.end())
        {
            bool found = false;
            for (auto& str : it->second)
            {
                if (TO_SID(str) == sid)
                    return true;
            }
        }

        return false;
    }

    double RuntimeInfo::GetElapsedTime()
    {
        return Time::GetCPUTime() - RuntimeInfo::s_startTime;
    }

    float RuntimeInfo::GetElapsedTimeF()
    {
        return static_cast<float>(GetElapsedTime());
    }
} // namespace Lina