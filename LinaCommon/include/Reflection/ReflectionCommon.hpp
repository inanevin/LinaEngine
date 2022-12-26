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

#ifndef CommonReflection_HPP
#define CommonReflection_HPP

// Headers here.

namespace Lina
{

// Title
#define LINA_CLASS(Title)

// IsAbstract is used to exclude certain reflection functions from registering to the type, such as creation & deletion.
// Mark your abstract classes w/ "True" for the reflection to work w/o compiler errors.
// 
// ChunkSize is used to know how much memory pool chunk will be allocated for the component. Empty will default to default value.
// Keep it the size of components you expect to be alive in a single world instance.

// Title, Category, optionals = IsAbstract, ChunkSize
#define LINA_COMPONENT(Title, Category, ...)

// Title, Type, Tooltip, Dependency, Category
#define LINA_FIELD(...)
}

#endif
