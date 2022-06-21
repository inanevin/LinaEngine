#include "Audio/Audio.hpp"
#include "Core/ResourceStorage.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <cereal/archives/portable_binary.hpp>
#include <fstream>

namespace Lina::Audio
{
    Audio::~Audio()
    {
        alDeleteBuffers(1, &m_buffer);
    }

    void* Audio::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        LINA_TRACE("[Audio Loader - Memory] -> Loading: {0}", path);
        ALsizei size;
        ALfloat freq;
        ALenum  format;
        ALvoid* aldata = alutLoadMemoryFromFileImage(data, (ALsizei)dataSize, &format, &size, &freq);

        ALenum err = alutGetError();
        LINA_ASSERT(err == ALUT_ERROR_NO_ERROR, "[Audio Loader] -> Failed loading audio from file memory: {0} {1}", path, alutGetErrorString(err));

        m_data           = aldata;
        m_format         = format;
        m_size           = size;
        m_freq           = freq;

        const String fileNameNoExt = Utility::GetFileWithoutExtension(path);
        const String assetDataPath = fileNameNoExt + ".linaaudiodata";
        GetCreateAssetdata<AudioAssetData>(assetDataPath, m_assetData);

        alGenBuffers((ALuint)1, &m_buffer);
        alBufferData(m_buffer, format, aldata, size, (ALsizei)freq);
        free(aldata);

#ifndef LINA_PRODUCTION_BUILD
        CheckForError();
#endif

        return static_cast<void*>(this);
    }

    void* Audio::LoadFromFile(const String& path)
    {
        LINA_TRACE("[Audio Loader - File] -> Loading: {0}", path);
        IResource::SetSID(path);
        ALsizei size;
        ALfloat freq;
        ALenum  format;
        ALvoid* data = alutLoadMemoryFromFile(path.c_str(), &format, &size, &freq);

        ALenum err = alutGetError();
        LINA_ASSERT(err == ALUT_ERROR_NO_ERROR, "[Audio Loader] -> Failed loading audio from file: {0} {1}", path, alutGetErrorString(err));

        m_data   = data;
        m_format = format;
        m_size   = size;
        m_freq   = freq;

        const String fileNameNoExt = Utility::GetFileWithoutExtension(path);
        const String assetDataPath = fileNameNoExt + ".linaaudiodata";
        GetCreateAssetdata<AudioAssetData>(assetDataPath, m_assetData);

        alGenBuffers((ALuint)1, &m_buffer);
        alBufferData(m_buffer, format, data, size, (ALsizei)freq);
        free(data);

#ifndef LINA_PRODUCTION_BUILD
        CheckForError();
#endif

        return static_cast<void*>(this);
    }

    void Audio::CheckForError()
    {
        ALCenum error;
        error = alGetError();
        if (error != AL_NO_ERROR)
        {
            LINA_ERR("[Audio Engine OpenAL] -> Open AL Error {0}", alutGetErrorString(error));
        }
    }
} // namespace Lina::Audio
