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
#include "Rendering/Shaders/Lina_BasicShader.hpp"  
#include "Core/Lina_Transform.hpp"
#include "Rendering/Lina_RenderingEngine.hpp"

Lina_BasicShader::Lina_BasicShader()
{
	Lina_Shader::Lina_Shader();	
}

void Lina_BasicShader::Init()
{
	Lina_Shader::Init();


	AddVertexShader(LoadShader("Lina_BasicVertex.vs"));
	AddFragmentShader(LoadShader("Lina_BasicFragment.fs"));
	CompileShader();

	AddUniform("model");
	AddUniform("color");
}

void Lina_BasicShader::UpdateUniforms(Lina_Transform& t, Lina_Material mat)
{
	Matrix4 world = t.GetTransformation();
	Matrix4 projected = RenderingEngine->GetCurrentActiveCamera()->GetViewProjection().Multiply(world);
	
	mat.texture.Bind();

	SetUniform("model", *(projected.m));
	SetUniform("color", mat.color);
	
}

