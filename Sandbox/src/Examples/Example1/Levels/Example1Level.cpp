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


void Example1Level::Install()
{
	LINA_CLIENT_WARN("Example level 1 install.");

	

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
