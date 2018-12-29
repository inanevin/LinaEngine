/*
Author: Inan Evin
www.inanevin.com

MIT License

Lina Engine, Copyright (c) 2018 Inan Evin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

4.0.30319.42000
9/30/2018 4:08:35 PM

*/

#pragma once

#ifndef Lina_Display_HPP
#define Lina_Display_HPP

#include "Core/Lina_SDLHandler.hpp"

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

	void SetFullScreen(int);
	void Update();
	void CloseWindow();

	bool IsClosed();
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }
	std::string GetTitle() { return m_Title; }
	void operator=(const Lina_Window& other) {}

private:

	int m_Width, m_Height;
	std::string m_Title;
	SDL_Window* m_Window;
	SDL_GLContext m_glContext;
	bool m_IsClosed;
};


#endif