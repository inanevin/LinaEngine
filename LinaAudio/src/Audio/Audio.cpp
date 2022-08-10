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

#include "Audio/Audio.hpp"
#include "Core/ResourceStorage.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Core/ResourceDataManager.hpp"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <fstream>

namespace Lina::Audio
{
    Audio::~Audio()
    {
        alDeleteBuffers(1, &m_buffer);
    }

    void* Audio::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        ALsizei size;
        ALfloat freq;
        ALenum  format;
        ALvoid* aldata = alutLoadMemoryFromFileImage(data, (ALsizei)dataSize, &format, &size, &freq);

        ALenum err = alutGetError();
        LINA_ASSERT(err == ALUT_ERROR_NO_ERROR, "[Audio Loader] -> Failed loading audio from file memory: {0} {1}", path, alutGetErrorString(err));

        m_data   = aldata;
        m_format = format;
        m_size   = size;
        m_freq   = freq;

        LoadAssetData();
        alGenBuffers((ALuint)1, &m_buffer);
        alBufferData(m_buffer, format, aldata, size, (ALsizei)freq);
        free(aldata);

#ifdef LINA_DEBUG
        CheckForError();
#endif

        return static_cast<void*>(this);
    }

    void* Audio::LoadFromFile(const String& path)
    {
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

        LoadAssetData();
        alGenBuffers((ALuint)1, &m_buffer);
        alBufferData(m_buffer, format, data, size, (ALsizei)freq);
        free(data);

#ifdef LINA_DEBUG
        CheckForError();
#endif

        return static_cast<void*>(this);
    }

    void Audio::LoadAssetData()
    {
        auto dm = Resources::ResourceDataManager::Get();
        if (!dm->Exists(m_sid))
            SaveAssetData();

        m_assetData.dummy = Resources::ResourceDataManager::Get()->GetValue<int>(m_sid, "Dummy");
    }

    void Audio::SaveAssetData()
    {
        auto* dm = Resources::ResourceDataManager::Get();
        dm->CleanSlate(m_sid);
        dm->SetValue<int>(m_sid, "Dummy", 0);
        dm->Save();
    }

    void Audio::CheckForError()
    {
        ALCenum error;
        error = alGetError();
        if (error != AL_NO_ERROR)
        {
            LINA_ERR("[Audio Engine OpenAL] -> Open AL Error: {0}", alutGetErrorString(error));
        }
    }
} // namespace Lina::Audio
