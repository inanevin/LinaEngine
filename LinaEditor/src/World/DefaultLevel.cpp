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

#include "World/DefaultLevel.hpp"
#include "Core/Application.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Utility/UtilityFunctions.hpp"

using namespace LinaEngine::Graphics;



namespace LinaEditor
{

	DefaultLevel::~DefaultLevel()
	{
		
	}

	bool DefaultLevel::Install(bool loadFromFile, const std::string& path, const std::string& levelName)
	{
		return true;
	}

	void DefaultLevel::Initialize()
	{
		// Create a simple procedural skybox.
		LinaEngine::Graphics::RenderEngine& renderEngine = LinaEngine::Application::GetRenderEngine();
		Material& mat = renderEngine.CreateMaterial(Shaders::SKYBOX_PROCEDURAL);
		mat.SetColor("material.startColor", Color(0.8f, 0.8f, 0.8f,1.0f));
		mat.SetColor("material.endColor", Color(0.2f, 0.2f, 0.2f));
		mat.SetVector3("material.sunDirection", Vector3(0.0f, -1.0f, 0.0f));
		renderEngine.SetSkyboxMaterial(mat);
	
	}


}