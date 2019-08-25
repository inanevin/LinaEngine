#include "..\..\..\..\include\Examples\Example1\Levels\Example1Level.hpp"
/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Example1Level
Timestamp: 5/6/2019 9:22:56 PM

*/


RenderableObjectData* cube1;
RenderableObjectData* cube2;
Texture* text1;
Texture* text2;
EntityHandle entity;
MeshRendererComponent renderableMesh;
TransformComponent transformComponent;

Texture skybox1;
Texture skybox2;
Texture skybox3;
Texture skybox4;

void Example1Level::Install()
{
	LINA_CLIENT_WARN("Example level 1 install.");

	skybox1 = m_RenderEngine->LoadCubemapTextureResource("Skybox/Skybox2/right.png", "Skybox/Skybox2/left.png", "Skybox/Skybox2/up.png", "Skybox/Skybox2/down.png", "Skybox/Skybox2/front.png", "Skybox/Skybox2/back.png");
	skybox2 = m_RenderEngine->LoadCubemapTextureResource("Skybox/Skybox3/right.png", "Skybox/Skybox3/left.png", "Skybox/Skybox3/up.png", "Skybox/Skybox3/down.png", "Skybox/Skybox3/front.png", "Skybox/Skybox3/back.png");
	skybox3 = m_RenderEngine->LoadCubemapTextureResource("Skybox/Skybox4/right.png", "Skybox/Skybox4/left.png", "Skybox/Skybox4/up.png", "Skybox/Skybox4/down.png", "Skybox/Skybox4/front.png", "Skybox/Skybox4/back.png");
	skybox4 = m_RenderEngine->LoadCubemapTextureResource("Skybox/Skybox5/right.png", "Skybox/Skybox5/left.png", "Skybox/Skybox5/up.png", "Skybox/Skybox5/down.png", "Skybox/Skybox5/front.png", "Skybox/Skybox5/back.png");

	m_RenderEngine->SetSkyboxDraw(true);

}

void Example1Level::Initialize()
{
	LINA_CLIENT_WARN("Example level 1 initialize.");

	transformComponent.transform.SetLocation(Vector3F(0.0f, 0.0f, 5.0f));

	/*cube1 = &m_RenderEngine->LoadModelResource("cube.obj");
	text1 = &m_RenderEngine->LoadTextureResource("chicken.png", PixelFormat::FORMAT_RGB, true, false);
	renderableMesh.texture = text1;
	renderableMesh.vertexArray = cube1->GetVertexArray(0);

	entity = m_ECS->MakeEntity(transformComponent, renderableMesh);*/
}


void Example1Level::Tick(float delta)
{
	if (m_InputEngine->GetKeyDown(InputCode::Key::K))
	{
		
	}
}
