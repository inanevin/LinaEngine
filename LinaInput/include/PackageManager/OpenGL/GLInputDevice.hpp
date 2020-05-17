/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: GLInputEngine
Timestamp: 4/14/2019 5:15:15 PM

*/

#pragma once

#ifndef GLInputDevice_HPP
#define GLInputDevice_HPP

#include "Utility/Math/Vector.hpp"
#include "Input/InputMappings.hpp"
#include "Input/InputCommon.hpp"

namespace LinaEngine::Input
{
	// A subclass of Input Engine. Check the InputEngine.hpp for method details.
	class GLInputDevice 
	{
	public:

		GLInputDevice();
		virtual ~GLInputDevice();

		// Initializes the device.
		void Initialize(void* contextWindowPointer);

		// Called each frame.
		void Tick();

		// Returns true each frame key mapped with the keyCode is pressed
		bool GetKey(int keyCode);

		// Returns true in the frame key mapped with the keyCode is pressed.
		bool GetKeyDown(int keyCode);

		// Returns true in the frame key mapped with the keyCode is stopped being pressed.
		bool GetKeyUp(int keyCode);

		// Returns true each frame mouse button mapped with the index is pressed
		bool GetMouseButton(int index);

		// Returns true in the frame mouse button mapped with the index is pressed. 
		bool GetMouseButtonDown(int index);

		// Returns true in the frame mouse mapped with the index is stopped being pressed.
		bool GetMouseButtonUp(int index);

		// Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Not smoothed.
		Vector2 GetRawMouseAxis();

		// Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Delta smoothed.
		Vector2 GetMouseAxis();

		// Returns a Vector2 containing screen space mouse positions
		Vector2 GetMousePosition();

		// Set cursor visible/invisible.
		void SetCursorMode(CursorMode mode) const;

		// Set mouse position.
		void SetMousePosition(const Vector2& v) const;

	private:

		int previousKeys[NUM_KEY_STATES];
		int currentKeys[NUM_KEY_STATES];
	};
}

#endif