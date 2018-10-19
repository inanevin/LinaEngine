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
10/18/2018 5:26:06 PM

*/

#include "pch.h"
#include "Rendering/Lina_BasicShader.h"  
#include "Utility/Lina_ResourceLoader.h"

Lina_BasicShader::Lina_BasicShader()
{
	Lina_Shader::Lina_Shader();	
}

void Lina_BasicShader::Init()
{
	Lina_Shader::Init();

	std::string vertexShaderText = Lina_ResourceLoader::LoadShader("Lina_BasicVertex.vs");
	std::string fragmentShaderText = Lina_ResourceLoader::LoadShader("Lina_BasicFragment.fs");

	AddVertexShader(vertexShaderText);
	AddFragmentShader(fragmentShaderText);
	CompileShader();

	AddUniform("transform");
	AddUniform("color");
}

void Lina_BasicShader::UpdateUniforms(Matrix4 world, Matrix4 projected, Vector3, Lina_Material mat)
{
	// UNBIND IF TEXTURE IS NULL?
	mat.texture.Bind();
	SetUniform("transform", *(projected.m));
	SetUniform("color", mat.color);
	
}

