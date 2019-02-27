/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: InputEngine_SDL
Timestamp: 1/6/2019 2:18:10 AM

*/

#pragma once

#ifdef LLF_INPUTANDWINDOW_SDL

#ifndef InputEngine_SDL_HPP
#define InputEngine_SDL_HPP

#include "Lina/Input/InputEngine.hpp"
#include "SDL.h"
#include "Lina/Events/Action.hpp"

namespace LinaEngine
{
	
	class LINA_API InputEngine_SDL : public InputEngine
	{
	public:

		InputEngine_SDL();
		~InputEngine_SDL();

		void Initialize() override;

		void OnUpdate() override;

		inline bool GetKey(int keyCode)            override { return *(m_CurrentKeys + keyCode); }
		inline bool GetKeyDown(int keyCode)        override { return (!(*(m_PreviousKeys + keyCode)) && (*(m_CurrentKeys + keyCode))); }
		inline bool GetKeyUp(int keyCode)          override { return (*(m_PreviousKeys + keyCode) && !(*(m_CurrentKeys + keyCode))); }
		inline bool GetMouse(int mouse)            override {
			if (mouse == 0)
				return mouse0Current;
			else if (mouse == 1)
				return mouse1Current;
			else if (mouse == 2)
				return mouse2Current;
		}
		inline bool GetMouseDown(int mouse)      override {
			if (mouse == 0)
				return !mouse0Previous && mouse0Current;
			else if (mouse == 1)
				return !mouse1Previous && mouse1Current;
			else if (mouse == 2)
				return !mouse2Previous && mouse2Current;
		}
		inline bool GetMouseUp(int mouse)        override {
			if (mouse == 0)
				return mouse0Previous && !mouse0Current;
			else if (mouse == 1)
				return mouse1Previous && !mouse1Current;
			else if (mouse == 2)
				return mouse2Previous && !mouse2Current;
		}
		inline Vector2F GetRawMouseAxis() override { return Vector2F(deltaMouseX, deltaMouseY); };
		inline Vector2F GetMouseAxis() override { return Vector2F(smoothDeltaMouseX, smoothDeltaMouseY); };
		inline Vector2F GetMousePosition() override { return Vector2F(currentMouseX, currentMouseY); }
	
		void SetCursor(bool visible) const override;
		void SetMousePosition(const Vector2F& v) const override;

	private:



		int numKeys = 512;
		static void SDLErrorCallback(const char* description);

		const Uint8* m_KeyboardState;
		Uint8* m_PreviousKeys;
		Uint8* m_CurrentKeys;

		float deltaMouseX;
		float deltaMouseY;
		float smoothDeltaMouseX;
		float smoothDeltaMouseY;
		float currentMouseX;
		float currentMouseY;
		int mouseXState;
		int mouseYState;
		bool mouse0Previous;
		bool mouse1Previous;
		bool mouse2Previous;
		bool mouse0Current;
		bool mouse1Current;
		bool mouse2Current;

		
	};
}


#endif

#endif