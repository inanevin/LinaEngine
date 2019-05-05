/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: TestLevel
Timestamp: 5/4/2019 3:54:37 PM

*/


#include "Examples/TestExample/Levels/TestLevel.hpp"  
#include "Rendering/ModelLoader.hpp"


RenderableMeshComponent* m_CubeMesh;
TransformComponent* m_CubeTransform;

ArrayBitmap m_CubeTextureBitmap;
Texture m_CubeTexture;
EntityHandle m_CubeEntity;
LinaArray<IndexedModel> m_Models;
LinaArray<uint32> modelMaterialIndices;

TestLevel::TestLevel()
{
	
}

void TestLevel::Install()
{
	m_CubeEntity = m_ECS->MakeEntity(TransformComponent(), RenderableMeshComponent());
	m_CubeTransform = (m_ECS->GetComponent<TransformComponent>(m_CubeEntity));
	m_CubeMesh = m_ECS->GetComponent<RenderableMeshComponent>(m_CubeEntity);

	m_CubeTextureBitmap.Load("Resources/Textures/checker.png");
	m_CubeTexture.Construct();
}

void TestLevel::Initialize()
{
	
//	ModelLoader::LoadModels("Resources/Mesh/cube.obj", m_Models, modelMaterialIndices, modelMaterials);


	m_CubeTransform->transform.SetLocation(Vector3F(2, 2, 2));
}

void TestLevel::Tick(float delta)
{
	std::cout << m_ECS->GetComponent<TransformComponent>(m_CubeEntity)->transform.GetLocation().ToString() << std::endl;
}
