/*
Author: Inan Evin
www.inanevin.com

BSD 2-Clause License
Lina Engine Copyright (c) 2018, Inan Evin All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.

-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO
-- THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
-- BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
-- GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
-- STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
-- OF SUCH DAMAGE.

4.0.30319.42000
9/30/2018 4:08:35 PM

*/

#pragma once

#ifndef Lina_Display_H
#define Lina_Display_H

#include <Lina_SDLHandler.h>
#undef main
#include "GL/glew.h"

class Lina_Window
{
public:

/*	static std::unique_ptr<Lina_Window> CreateDisplayWindow(int width, int height, const std::string& title)
	{
	  std::unique_ptr<Lina_Window> p(new Lina_Window(width,height,title));
	  return std::move(p);
	}
	*/

	Lina_Window(int, int, const std::string&);
	~Lina_Window();

	void Update();
	void Clear(float, float, float, float);
	void RenderBlankColor();


	bool IsClosed();
	inline int GetWidth() { return m_Width; }
	inline int GetHeight() { return m_Height; }
	inline std::string GetTitle() { return m_Title; }
	void operator=(const Lina_Window& other) {}



private:

	int m_Width, m_Height;
	std::string m_Title;
	SDL_Window* m_Window;
	SDL_GLContext m_glContext;
	bool m_IsClosed;
};


#endif