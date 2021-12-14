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
Class: ResourcePackages



Timestamp: 12/28/2020 9:53:53 PM
*/

#pragma once

#ifndef ResourcePackages_HPP
#define ResourcePackages_HPP

// Headers here.
#include "Utility/StringId.hpp"
#include "Resources/ImageResource.hpp"
#include "Resources/MeshResource.hpp"
#include "Resources/AudioResource.hpp"
#include "Resources/MaterialResource.hpp"
#include "Resources/ShaderResource.hpp"

namespace Lina::Resources
{
	typedef std::unordered_map<StringIDType, ImageResource*> ImagePackage;
	typedef std::unordered_map<StringIDType, MeshResource*> MeshPackage;
	typedef std::unordered_map<StringIDType, AudioResource*> AudioPackage;
	typedef std::unordered_map<StringIDType, MaterialResource*> MaterialPackage;
	typedef std::unordered_map<StringIDType, ShaderResource*> ShaderPackage;
	typedef std::unordered_map<StringIDType, std::vector<unsigned char>> RawPackage;
}

#endif
