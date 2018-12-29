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
9/30/2018 4:08:49 PM

*/

#include "pch.h"
#include "Rendering/Lina_Window.hpp"  

Lina_Window::Lina_Window(int width, int height, const std::string& title) : m_Width(width), m_Height(height), m_Title(title)
{
	// Add a console message about correct initialization.
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Window initialized. (W: " + std::to_string(width) + " H: " + std::to_string(height) + ")", Lina_Console::MsgType::Initialization, "Window");

	// Set additional parameters for SDL window using SDL_WINDOW_OPENGL
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);	// 8 bits (at least) -> 2 to the pow of 8 amount of color data. 256.
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);	// 8 bits -> 2 to the pow of 8 amount of color data. 256.
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);	// 8 bits -> 2 to the pow of 8 amount of color data. 256.
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);	// 8 bits -> 2 to the pow of 8 amount of color data. 256.
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);	// How much data will SDL allocate for a single pixel.
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);	// Alloc an area for 2 blocks of display mem.


	// Create an SDL window.
	m_Window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);

	/* SDL -> Goes to OS, asks a window with the params. But we can not draw via OpenGL on the returned window bc OS has control over it.
		That's where the GL context comes into play. Reduces the control of OS over a window, so SDL -> Goes to OS, OS -> goes to GPU while
		OpenGL commands the GPU about this transfer, then GPU initalizes the window so that it can draw on it using openGL commands.
	*/

	// We create a context using our window, so we will have power over our window via OpenGL -> GPU.
	m_glContext = SDL_GL_CreateContext(m_Window);

	// Disable vsync. Needs to be called after SDL_GL_CreateContext bc swap interval works on the current active context. ( can be changed laterwards )
	SDL_GL_SetSwapInterval(0);

	// Initialize GLEW.
	GLenum status = glewInit();
	
	// Check glew initialization status.
	if (status != GLEW_OK)
		cons.AddConsoleMsg("Glew failed to initialize!", Lina_Console::MsgType::Error, "Window");
	else
		cons.AddConsoleMsg("Glew initialized.", Lina_Console::Initialization, "Window");

	// Set closed flag. This will be checked by OS events being received on Update.
	m_IsClosed = false;

}

Lina_Window::~Lina_Window()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Window deinitialized.", Lina_Console::MsgType::Deinitialization, "Window");

	// Deallocate GL context and window. (m_Window pointer is deleted via SDL_DestroyWindow already so no need to use delete again on that.)
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_Window);
}


void Lina_Window::CloseWindow()
{
	m_IsClosed = true;
}

void Lina_Window::SetFullScreen(int flag)
{
	SDL_SetWindowFullscreen(m_Window, flag);
}

void Lina_Window::Update()
{
	// We had allocated space for 2 windows space via SDL_GL_SetAttribute. First both of them are empty, our window will only display the first one.
	// OpenGL will draw to the invisible buffer, then we swap buffers, our window will start displaying the previously drawn buffer while
	// OpenGL will start drawing on the swaped buffer. And so on.
	// So our window will never display a buffer that is currently being drawn by opengl.
	SDL_GL_SwapWindow(m_Window);
}

bool Lina_Window::IsClosed() { return m_IsClosed; }

/* NOTE ABOUT GPU's & PIPELINE 

// GPU can -> draw a triangle, interpolates bw points, pass data in parallel.
// GPU takes data (in parallel, so it takes every individual unit of data), processes it (converts it to some standart form for itself)
// Processed data form is, dots, dots that CAN be converted into a triangle. This stage is called the -> "VERTEX SHADER".
// Rasterization -> Connect the dots into triangles. After connecting them, it will fill the triangles in.
// Take individual pixels from the triangles, and process them. 
// For every individual pixel in the triangles, it generates colors. This stage of processing, the coloring, is called -> "FRAGMENT SHADER"
// It takes your output image and writes all the pixels on it.
// There will be a lot of pipelines working in paralel to write onto the output image.
// After all the parallel pipelines finished, the output image is sent to the display.

// We send data to GPU, but while doing so we have to tell how to interpret it.
// We are gonna create a vertex shader & fragment shader. Then pass them to GPU so that it can generate in the way we want.

*/