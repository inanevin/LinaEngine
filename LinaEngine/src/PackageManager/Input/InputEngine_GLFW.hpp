/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: InputEngine_GLFW
Timestamp: 2/25/2019 9:43:54 AM

*/

#pragma once

#ifdef LLF_INPUTANDWINDOW_GLFW

#ifndef InputEngine_GLFW_HPP
#define InputEngine_GLFW_HPP

#include "Lina/Input/InputEngine.hpp"
#include "PackageManager/Graphics/OpenGL/Window/Window_GLFWGL.hpp"

namespace LinaEngine
{
	class InputEngine_GLFW : public InputEngine
	{
	public:

		~InputEngine_GLFW();

		/* Initializes input engine */
		void Initialize() override;

		/* Called when updating the input engine. */
		void OnUpdate() override;

		/* Returns true each frame key mapped with the keyCode is pressed */
		bool GetKey(int keyCode) override;

		/* Returns true in the frame key mapped with the keyCode is pressed. */
		bool GetKeyDown(int keyCode) override;

		/* Returns true in the frame key mapped with the keyCode is stopped being pressed. */
		bool GetKeyUp(int keyCode) override;

		/* Returns true each frame mouse button mapped with the index is pressed */
		bool GetMouseButton(int index) override;

		/* Returns true in the frame mouse button mapped with the index is pressed. */
		bool GetMouseButtonDown(int index) override;

		/* Returns true in the frame mouse mapped with the index is stopped being pressed. */
		bool GetMouseButtonUp(int index) override;

		/* Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Not smoothed.*/
		Vector2F GetRawMouseAxis() override;

		/* Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Delta smoothed.*/
		Vector2F GetMouseAxis() override;

		Vector2F GetMousePosition() override;

		/* Sets the cursor visible.*/
		void SetCursor(bool visible) const;

		/* Sets mouse position to desired screen space coordinates. */
		void SetMousePosition(const Vector2F& v) const;

		void DispatchKeyAction(int  key, int action) override;
		void DispatchMouseAction(int button, int action) override;



	private:


		GLFWwindow* glfwWindow;
		int previousKeys[NUM_KEY_STATES];
		int currentKeys[NUM_KEY_STATES];
	
	};
}


#endif

#endif