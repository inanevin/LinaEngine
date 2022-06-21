#include "Core/AudioEngine.hpp"

#include "Audio/Audio.hpp"
#include "ECS/Registry.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <fstream>

namespace Lina::Audio
{
    AudioEngine* AudioEngine::s_audioEngine = nullptr;

    void AudioEngine::Initialize()
    {
        LINA_TRACE("[Initialization] -> Audio Engine ({0})", typeid(*this).name());

        // List available devices.
        ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");

        if (enumeration == AL_FALSE)
            LINA_WARN("[Audio Engine OpenAL] -> Open AL enumeration is not present!");
        if (alcIsExtensionPresent(NULL, "ALC_enumeration_EXT") == AL_TRUE)
        {
            char* s;

            if (alcIsExtensionPresent(NULL, "ALC_enumerate_all_EXT") == AL_FALSE)
                s = (char*)alcGetString(NULL, ALC_DEVICE_SPECIFIER);
            else
                s = (char*)alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);

            ListAudioDevices("Output", s);

            s = (char*)alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
            ListAudioDevices("Input", s);
        }
        else
            ListAudioDevices("Input", alcGetString(NULL, ALC_DEVICE_SPECIFIER));

        const ALCchar* defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

        // Init device.
        m_device = alcOpenDevice(defaultDeviceName);
        LINA_TRACE("[Audio Engine OpenAL] -> Initialized {0}", alcGetString(m_device, ALC_ALL_DEVICES_SPECIFIER));

        if (!m_device)
        {
            LINA_ERR("[Audio Engine OpenAL] -> Can not open ALC device!");
            return;
        }

        // Init context.
        m_context = alcCreateContext(m_device, NULL);
        if (!alcMakeContextCurrent(m_context))
        {
            LINA_ERR("[Audio Engine OpenAL] -> Open AL Context Error {0}", alutGetErrorString(alGetError()));
        }

        // Init alut
        alutInit(NULL, NULL);
    }

    void AudioEngine::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Audio Engine ({0})", typeid(*this).name());

        for (auto& s : m_generatedSources)
            alDeleteSources((ALuint)1, &s.second);

        m_generatedSources.clear();
    }

    void AudioEngine::PlayOneShot(Audio* audio, float gain, bool looping, float pitch, Vector3 position, Vector3 velocity)
    {
        unsigned int source = -1;

        StringIDType sid = audio->GetSID();

        if (m_generatedSources.find(sid) == m_generatedSources.end())
        {
            alGenSources((ALuint)1, &source);
            m_generatedSources[sid] = source;
        }
        else
            source = m_generatedSources[sid];

        alSourcef(source, AL_PITCH, pitch);
        alSourcef(source, AL_GAIN, gain);
        alSource3f(source, AL_POSITION, position.x, position.y, position.z);
        alSource3f(source, AL_VELOCITY, 0, 0, 0);
        alSourcei(source, AL_LOOPING, looping);
        alSourcei(source, AL_BUFFER, audio->GetBuffer());
        alSourcePlay(source);
    }

    void AudioEngine::ListAudioDevices(const char* type, const char* list)
    {
        ALCchar *ptr, *nptr;
        ptr = (ALCchar*)list;
        LINA_TRACE("[Audio Engine OpenAL] -> List of all available {0} devices:", type);

        if (!list)
        {
            LINA_INFO("[Audio Engine OpenAL] -> None");
        }
        else
        {
            nptr = ptr;
            while (*(nptr += strlen(ptr) + 1) != 0)
            {
                LINA_INFO("[Audio Engine OpenAL] -> {0}", ptr);
                ptr = nptr;
            }
            LINA_INFO("[Audio Engine OpenAL] -> {0}", ptr);
        }
    }

  
} // namespace Lina::Audio
