/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: GraphicsAdapter
Timestamp: 12/31/2018 1:46:13 AM

*/

#pragma once

#ifndef GraphicsAdapter_HPP
#define GraphicsAdapter_HPP

#ifdef LLF_GRAPHICS_OPENGL

	#include "RenderingEngine_OpenGL.hpp"
	#define RENDERINGENGINE_CREATEFUNC(PARAM) inline RenderingEngine* CreateRenderingEngine() { return new RenderingEngine_OpenGL(); }

	#ifdef LLF_INPUTANDWINDOW_SDL

		#include "Window_SDLGL.hpp"
		#define WINDOWCREATEFUNC(PARAM) inline Window* CreateEngineWindow(const WindowProps& props = WindowProps()) { return Window_SDLGL::Create(props); }

	#else

		#include "Lina/Rendering/Window.hpp"

		#define WINDOWCREATEFUNC(PARAM) inline Window* CreateEngineWindow(const WindowProps& props = WindowProps()) { LINA_CORE_ERR("No LLF is defined for Graphics Handling! Aborting."); exit(EXIT_FAILURE); }

	#endif

#else

	#include "Lina/Rendering/RenderingEngine.hpp"

	#define RENDERINGENGINE_CREATEFUNC(PARAM) inline RenderingEngine* CreateRenderingEngine() { LINA_CORE_ERR("No LLF is defined for rendering! Aborting!");  exit(EXIT_FAILURE); };

#endif 





namespace LinaEngine
{
	class GraphicsAdapter
	{

	public:

		GraphicsAdapter();
		~GraphicsAdapter() {};
		

		RENDERINGENGINE_CREATEFUNC(PARAM);
		WINDOWCREATEFUNC(PARAM);
	};
}



#endif