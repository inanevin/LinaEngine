/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: GLGraphicsDefines
Timestamp: 4/26/2019 12:50:33 AM

*/

#pragma once

#ifndef GLGraphicsDefines_HPP
#define GLGraphicsDefines_HPP

#include "glad/glad.h"

enum BufferUsage
{
	USAGE_STATIC_DRAW = GL_STATIC_DRAW,
	USAGE_STREAM_DRAW = GL_STREAM_DRAW,
	USAGE_DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
	USAGE_STATIC_COPY = GL_STATIC_COPY,
	USAGE_STREAM_COPY = GL_STREAM_COPY,
	USAGE_DYNAMIC_COPY = GL_DYNAMIC_COPY,
	USAGE_STATIC_READ = GL_STATIC_READ,
	USAGE_STREAM_READ = GL_STREAM_READ,
	USAGE_DYNAMIC_READ = GL_DYNAMIC_READ,
};




#endif