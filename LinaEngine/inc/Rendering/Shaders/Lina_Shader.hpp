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
10/15/2018 7:16:21 PM

*/

#pragma once

#ifndef Lina_Shader_HPP
#define Lina_Shader_HPP

#include <GL/glew.h>
#include "Math/Lina_Math.hpp"
#include "Math/Lina_Vector3F.hpp"
#include "Math/Lina_Matrix4F.hpp"
#include "Rendering/Lina_Material.hpp"
#include <map>

class Lina_Transform;
class Lina_RenderingEngine;
class Lina_DirectionalLight;
class Lina_PointLight;
class Lina_SpotLight;


class Lina_Shader
{

public:

	Lina_Shader() {};

	virtual void Init();
	virtual void Bind();
	virtual void UpdateUniforms(Lina_Transform&, Lina_Material);
	
	void AddVertexShader(std::string);
	void AddGeometryShader(std::string);
	void AddFragmentShader(std::string);
	void CompileShader();
	void AddUniform(const std::string&);
	void SetUniform(const std::string&, float) const;
	void SetUniform(const std::string&, int) const;
	void SetUniform(const std::string&, Vector3) const;
	void SetUniform(const std::string&, GLfloat*) const;
	void SetUniform(const std::string&, const Lina_DirectionalLight&) const;
	void SetUniform(const std::string&, const Lina_PointLight&) const;
	void SetUniform(const std::string&, const Lina_SpotLight&) const;

	void CheckError(unsigned int, int, std::string);
	void SetAttributeLocation(int, std::string);

protected:

	std::string LoadShader(std::string);
	Lina_RenderingEngine* RenderingEngine;

private:

	friend class Lina_RenderingEngine;

	inline void SetRenderingEngine(Lina_RenderingEngine* eng) { std::cout << &eng; RenderingEngine = eng; };
	void AddToProgram(std::string, GLint); 

	unsigned int program;
	std::map<std::string, int> m_Uniforms;

};




#endif