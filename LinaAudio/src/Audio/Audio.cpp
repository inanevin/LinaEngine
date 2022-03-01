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
#include "Resources/ResourceStorage.hpp"
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

    void* Audio::LoadFromMemory(const std::string& path, unsigned char* data, size_t dataSize)
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

        const std::string fileNameNoExt = Utility::GetFileWithoutExtension(path);
        const std::string assetDataPath = fileNameNoExt + ".linaaudiodata";
        GetCreateAssetdata<AudioAssetData>(assetDataPath, m_assetData);

        alGenBuffers((ALuint)1, &m_buffer);
        alBufferData(m_buffer, format, aldata, size, (ALsizei)freq);
        free(aldata);

#ifndef LINA_PRODUCTION_BUILD
        CheckForError();
#endif

        return static_cast<void*>(this);
    }

    void* Audio::LoadFromFile(const std::string& path)
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

        const std::string fileNameNoExt = Utility::GetFileWithoutExtension(path);
        const std::string assetDataPath = fileNameNoExt + ".linaaudiodata";
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
