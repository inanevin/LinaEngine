/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: DefaultLevel
Timestamp: 10/16/2020 12:58:11 AM

*/


#include "World/DefaultLevel.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Utility/UtilityFunctions.hpp"

using namespace LinaEngine::Graphics;

namespace LinaEditor
{
	bool DefaultLevel::Install()
	{

		return true;
	}

	void DefaultLevel::Initialize()
	{
		// Create a simple procedural skybox.
		Material& mat = m_renderEngine->CreateMaterial(Utility::GetUniqueID(), Shaders::SKYBOX_PROCEDURAL);
		mat.SetColor("material.startColor", Color(0.8f, 0.8f, 0.8f,1.0f));
		mat.SetColor("material.endColor", Color(0.2f, 0.2f, 0.2f));
		mat.SetVector3("material.sunDirection", Vector3(0.0f, -1.0f, 0.0f));
		m_renderEngine->SetSkyboxMaterial(mat);
	}
}