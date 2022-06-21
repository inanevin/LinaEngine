/*
Class:  Audio

Responsible for initializing, running and cleaning up the audio world.

Timestamp: 5/1/2019 2:35:28 AM
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
