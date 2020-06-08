/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: SB_Application
Timestamp: 12/29/2018 11:15:41 PM

*/

#include <Lina.hpp>
#include <Utility/Log.hpp>
#include "Examples/Example1/Levels/Example1Level.hpp"

class TestLayer : public LinaEngine::Layer
{
public:

	TestLayer() : Layer("Test")
	{
	
	}

	void OnUpdate() override
	{
		//LINA_CLIENT_INFO("TestLayer Update");
	}

	void OnEvent() override
	{
		//LINA_CLIENT_TRACE("{0}", e);

	}
};


class Sandbox : public LinaEngine::Application
{
public:
	Sandbox() { 
		//LINA_CLIENT_TRACE("[Constructor] -> Sandbox ({0})", typeid(*this).name());
	
		//LinaEngine::Layer* l = new TestLayer();
		//PushLayer(l);
		//PushOverlay(new LinaEngine::Layer_IMGUI());
		
		LoadLevel(&level);
	}
	~ Sandbox(){
		//LINA_CLIENT_TRACE("[Destructor] -> Sanbox ({0})", typeid(*this).name());
	}

	Example1Level level;

	
};


LinaEngine::Application* LinaEngine::CreateApplication()
{
	return new Sandbox();
}

