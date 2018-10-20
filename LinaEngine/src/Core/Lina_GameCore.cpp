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
10/1/2018 6:00:05 AM

*/

#include "pch.h"
#include "Core/Lina_GameCore.h"  
#include "Utility/Lina_ResourceLoader.h"
#include "Math/Lina_Math.h"
#include "Utility/Lina_Time.h"

void Lina_GameCore::Wake()
{
	m_TestScene.Wake();

}

void Lina_GameCore::Start()
{
	m_TestScene.Start();
}

void Lina_GameCore::ProcessInput()
{
	m_TestScene.ProcessInput();
}


void Lina_GameCore::Update()
{
	m_TestScene.Update();
}



void Lina_GameCore::Render()
{
	m_TestScene.Render();

}


