/*
Author: Inan Evin
www.inanevin.com

MIT License

Lina Engine, Copyright (c) 2018 Inan Evin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

4.0.30319.42000
10/23/2018 11:25:18 PM

*/

#pragma once

#ifndef Lina_ECSSystem_HPP
#define Lina_ECSSystem_HPP

#include "Lina_ECSComponent.hpp"

class Lina_ECSBaseSystem
{
public:
	Lina_ECSBaseSystem(const Lina_DSArray<uint32>& componentTypesIn) : componentTypes(componentTypesIn) {}
	virtual void UpdateComponents(float delta, Lina_ECSBaseComponent** components) {}
	const Lina_DSArray<uint32>& GetComponentTypes()
	{
		return componentTypes;
	}
private:
	Lina_DSArray<uint32> componentTypes;
};

#endif