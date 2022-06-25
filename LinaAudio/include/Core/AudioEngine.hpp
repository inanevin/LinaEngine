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

#ifndef AudioEngine_HPP
#define AudioEngine_HPP

#include "Math/Vector.hpp"
#include "Utility/StringId.hpp"
#include "Data/HashMap.hpp"
#include <mutex>

struct ALCcontext;
struct ALCdevice;

namespace Lina
{
    class Engine;

    namespace World
    {
        class Level;
    }

    namespace Event
    {
        struct ELoadResourceFromFile;
        struct ELoadResourceFromMemory;
    } // namespace Event
} // namespace Lina

namespace Lina::Audio
{
    class Audio;

    class AudioEngine
    {
    public:
        void                PlayOneShot(Audio* audio, float gain = 1.0f, bool looping = false, float pitch = 1.0f, Vector3 position = Vector3::Zero, Vector3 velocity = Vector3::Zero);
        static AudioEngine* Get()
        {
            return s_audioEngine;
        }

    private:
        void ListAudioDevices(const char* type, const char* list);

    private:
        friend class Engine;
        AudioEngine()  = default;
        ~AudioEngine() = default;
        void Initialize();
        void Shutdown();

    private:
        static AudioEngine*                 s_audioEngine;
        mutable std::mutex                  m_mutex;
        ALCcontext*                         m_context             = nullptr;
        ALCdevice*                          m_device              = nullptr;
        Vector3                             m_mainListenerLastPos = Vector3::Zero;
        HashMap<StringIDType, unsigned int> m_generatedSources;
    };
} // namespace Lina::Audio

#endif
