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


#include "Lina/Input/InputEngine.hpp"


namespace LinaEngine
{
	class GLInputEngine : public InputEngine<GLInputEngine>
	{
	public:

		GLInputEngine();
		virtual ~GLInputEngine();

		/* Initializes input renderDevice */
		void Initialize_Impl(void* contextWindowPointer);

		/* Called each frame */
		void Tick_Impl();

		/* Returns true each frame key mapped with the keyCode is pressed */
		bool GetKey_Impl(int keyCode);

		/* Returns true in the frame key mapped with the keyCode is pressed. */
		bool GetKeyDown_Impl(int keyCode);

		/* Returns true in the frame key mapped with the keyCode is stopped being pressed. */
		bool GetKeyUp_Impl(int keyCode);

		/* Returns true each frame mouse button mapped with the index is pressed */
		bool GetMouseButton_Impl(int index);

		/* Returns true in the frame mouse button mapped with the index is pressed. */
		bool GetMouseButtonDown_Impl(int index);

		/* Returns true in the frame mouse mapped with the index is stopped being pressed. */
		bool GetMouseButtonUp_Impl(int index);

		/* Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Not smoothed.*/
		Vector2F GetRawMouseAxis_Impl();

		/* Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Delta smoothed.*/
		Vector2F GetMouseAxis_Impl();

		/* Returns a Vector2 containing screen space mouse positions */
		Vector2F GetMousePosition_Impl();

		/* Sets the cursor visible.*/
		void SetCursor_Impl(bool visible) const;

		/* Sets mouse position to desired screen space coordinates. */
		void SetMousePosition_Impl(const Vector2F& v) const;
	

	private:

		/* Key data */
		int previousKeys[NUM_KEY_STATES];
		int currentKeys[NUM_KEY_STATES];

	};
}


#endif