/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Ýnan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Lina_Application
Timestamp: 12/29/2018 10:43:46 PM

*/

#include "LinaPch.hpp"
#include "Application.hpp"  
#include "Events/ApplicationEvent.hpp"
#include <SDL.h>
#include "SDL_events.h"
SDL_Event event;
namespace LinaEngine
{
	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::Run()
	{

	

		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
			
		}

		SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
		if (win == nullptr) {
			std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
			SDL_Quit();
			
		}

		while (true)
		{
			while (SDL_PollEvent(&event)) {
				//If user closes the window
				if (event.type == SDL_QUIT) {
					
				}
				//If user presses any key
				if (event.type == SDL_KEYDOWN) {
					LINA_CORE_TRACE("KP");
				}
				//If user clicks the mouse
				if (event.type == SDL_MOUSEBUTTONDOWN) {
					LINA_CORE_TRACE("MP");
				}
			}
		}
	}
}

