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

Class: InputDevice
Timestamp: 10/6/2020 11:13:50 AM

*/
#pragma once

#ifndef InputDevice_HPP
#define InputDevice_HPP

// Headers here.

#include "Utility/Math/Vector.hpp"
#include "Input/InputMappings.hpp"
#include "Input/InputCommon.hpp"

namespace LinaEngine::Input
{
	class InputDevice
	{
		
	public:
		
		InputDevice() {};
		~InputDevice() {};

		// Initializes the device.
		virtual void Initialize(void* contextWindowPointer) = 0;
		
		// Called each frame.
		virtual void Tick()=0;
		
		// Returns true each frame key mapped with the keyCode is pressed
		virtual bool GetKey(int keyCode) = 0;
		
		// Returns true in the frame key mapped with the keyCode is pressed.
		virtual bool GetKeyDown(int keyCode) = 0;
		
		// Returns true in the frame key mapped with the keyCode is stopped being pressed.
		virtual bool GetKeyUp(int keyCode) = 0;
		
		// Returns true each frame mouse button mapped with the index is pressed
		virtual bool GetMouseButton(int index) = 0;
		
		// Returns true in the frame mouse button mapped with the index is pressed. 
		virtual bool GetMouseButtonDown(int index) = 0;
		 
		// Returns true in the frame mouse mapped with the index is stopped being pressed.
		virtual bool GetMouseButtonUp(int index) = 0;
		
		// Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Not smoothed.
		virtual Vector2 GetRawMouseAxis()=0;
		
		// Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Delta smoothed.
		virtual Vector2 GetMouseAxis()=0;
		
		// Returns a Vector2 containing screen space mouse positions
		virtual Vector2 GetMousePosition()=0;
		
		// Set cursor visible/invisible.
		virtual void SetCursorMode(CursorMode mode) const = 0;
	
		// Set mouse position.
		virtual void SetMousePosition(const Vector2& v) const = 0;
	
	private:
	
	};
}

#endif
