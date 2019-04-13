/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: InputEngine
Timestamp: 1/6/2019 2:17:55 AM

*/

#pragma once
#ifndef InputEngine_HPP
#define InputEngine_HPP

#include "Lina/Events/ActionDispatcher.hpp"

namespace LinaEngine
{

#define MAX_AXIS_AMOUNT 10

	class Application;

	class LINA_API InputEngine : public ActionDispatcher
	{

	public:

		virtual ~InputEngine();

		/* Called when input engine is initialized. */
		virtual void Initialize();

		/* Called when updating the input engine. */
		virtual void OnUpdate() = 0;

		/* Returns true each frame key mapped with the keyCode is pressed */
		virtual bool GetKey(int keyCode) = 0;

		/* Returns true in the frame key mapped with the keyCode is pressed. */
		virtual bool GetKeyDown(int keyCode) = 0;

		/* Returns true in the frame key mapped with the keyCode is stopped being pressed. */
		virtual bool GetKeyUp(int keyCode) = 0;

		/* Returns true each frame mouse button mapped with the index is pressed */
		virtual bool GetMouseButton(int index) = 0;

		/* Returns true in the frame mouse button mapped with the index is pressed. */
		virtual bool GetMouseButtonDown(int index) = 0;

		/* Returns true in the frame mouse mapped with the index is stopped being pressed. */
		virtual bool GetMouseButtonUp(int index) = 0;

		/* Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Not smoothed.*/
		virtual Vector2F GetRawMouseAxis() = 0;

		/* Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Delta smoothed.*/
		virtual Vector2F GetMouseAxis() = 0;

		/* Returns the mouse position as Vector2 */
		virtual Vector2F GetMousePosition() = 0;

		/* Sets the cursor visible.*/
		virtual void SetCursor(bool visible) const = 0;

		/* Sets mouse position to desired screen space coordinates. */
		virtual void SetMousePosition(const Vector2F& v) const = 0;

		/* Dispatches the key actions */
		virtual void DispatchKeyAction(Input::Key key, int action) = 0;

		/* Dispatches the mouse actions */
		virtual void DispatchMouseAction(Input::Mouse button, int action) = 0;

		/* Sets the Application reference */
		FORCEINLINE void SetApplication(Application& const application) { app = &application; }

		/* Returns the Horizontal Axis */
		class InputKeyAxisBinder* GetHorizontalInput();

		/* Returns the Vertical Axis */
		class InputKeyAxisBinder* GetVerticalInput();

	protected:

		InputEngine();

		/* Horizontal Input Axis */
		class InputKeyAxisBinder* m_HorizontalAxis;

		/* Vertical Input Axis */
		class InputKeyAxisBinder* m_VerticalAxis;

		/* App reference */
		Application* app;
	};


}


#endif