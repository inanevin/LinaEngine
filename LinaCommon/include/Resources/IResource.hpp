/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

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
Class: IResource



Timestamp: 12/30/2021 9:37:24 PM
*/

#pragma once

#ifndef IResource_HPP
#define IResource_HPP

// Headers here.
#include "Core/SizeDefinitions.hpp"
#include <string>
#include <cereal/archives/portable_binary.hpp>

namespace Lina::Resources
{
    class IResource
    {
    public:
        IResource()          = default;
        virtual ~IResource() = default;

        virtual void LoadFromMemory(unsigned char* data, size_t dataSize, const std::string& idPath){};
        virtual void LoadFromFile(const std::string& path){};
        
        template<typename T>
        T LoadArchiveFromFile(const std::string& path)
        {
            T obj;
            std::ifstream  stream(path);
            {
                cereal::PortableBinaryInputArchive iarchive(stream);
                iarchive(obj);
            }
            return obj;
        }

        template<typename T>
        T LoadArchiveFromMemory(unsigned char* data, size_t dataSize)
        {
            T obj;
            {
                std::string        data((char*)data, dataSize);
                std::istringstream stream(data);
                {
                    cereal::PortableBinaryInputArchive iarchive(stream);
                    iarchive(obj);
                }
            }
            return obj;
        }

        template<typename T>
        void SaveArchiveToFile(const std::string& path, T& obj)
        {
            std::ofstream stream(path);
            {
                cereal::PortableBinaryOutputArchive oarchive(stream);
                oarchive(assetData);
            }
        }
    };
} // namespace Lina::Resources

#endif
