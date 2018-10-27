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
10/18/2018 9:59:19 PM

*/

#pragma once

#ifndef Lina_BaseLight_HPP
#define Lina_BaseLight_HPP


#include "Game/Lina_ActorComponent.hpp"
#include "DataStructures/Lina_Color.hpp"

class Lina_Shader;

class Lina_BaseLight : public Lina_ActorComponent
{

public:
	
	Color color;
	float intensity;

protected:

	Lina_BaseLight() {};
	Lina_BaseLight(Color c, float i, Lina_Shader* s) : color(c), intensity(i), m_Shader(s) {};
	Lina_BaseLight(Lina_Shader* s) : color(COLOR_Black), intensity(0.0f), m_Shader(s) {};

private:

	Lina_Shader* m_Shader;

};



#endif