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
10/19/2018 4:53:13 PM

*/

#pragma once

#ifndef Lina_Attenuation_HPP
#define Lina_Attenuation_HPP

class Lina_Attenuation
{

public:

	Lina_Attenuation() : constant(0), linear(0), exponent(1.0f) {};
	Lina_Attenuation(float c, float l, float e) : constant(c), linear(l), exponent(e) {};


	float constant;
	float linear;
	float exponent;

};

static Lina_Attenuation AT_CONSTANT = Lina_Attenuation(1.0f, 0.0f, 0.0f);
static Lina_Attenuation AT_CONSTLIN = Lina_Attenuation(0.66f, 0.33f, 0.0f);
static Lina_Attenuation AT_CONSTQUAD = Lina_Attenuation(0.66f, 0.0f, 0.33f);
static Lina_Attenuation AT_LINEAR = Lina_Attenuation(0.0f, 1.0f, 0.0f);
static Lina_Attenuation AT_LINCONST = Lina_Attenuation(0.33f, 0.66f, 0.0f);
static Lina_Attenuation AT_LINQUAD = Lina_Attenuation(0.0f, 0.66f, 0.33f);
static Lina_Attenuation AT_QUADRATIC = Lina_Attenuation(0.0f, 0.0f, 1.0f);
static Lina_Attenuation AT_QUADCONST = Lina_Attenuation(0.33f, 0.0f, 0.66f);
static Lina_Attenuation AT_QUADLIN = Lina_Attenuation(0.0f, 0.33f, 0.66f);

#endif