/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: RenderEngine
Timestamp: 4/15/2019 12:26:31 PM

*/

#pragma once

#ifndef RenderEngine_HPP
#define RenderEngine_HPP

#include "Events/Event.hpp"
#include "Core/DataStructures.hpp"
#include "PackageManager/PAMRenderDevice.hpp"


namespace LinaEngine::Graphics
{


	class RenderEngine
	{
	public:

		RenderEngine();
		~RenderEngine();
		// Get a void* reference to the native window. e.g GLFWwindow
		FORCEINLINE void* GetNativeWindow() { return m_RenderDevice->GetNativeWindow(); }

		// Initialize the render renderEngine.
		void Initialize();

		// Called each frame.
		void Tick();

		// Create a render context.
		FORCEINLINE bool CreateContextWindow() { return m_RenderDevice->CreateContextWindow(); };

		// Set the target of the callbacks coming from the main window context.
		FORCEINLINE void SetMainWindowEventCallback(const std::function<void(Event&)>& callback) { m_RenderDevice->SetMainWindowEventCallback(callback); }

	private:

		std::unique_ptr<PAMRenderDevice> m_RenderDevice;


	};

}


#endif