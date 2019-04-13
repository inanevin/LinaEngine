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

#include "LinaPch.hpp"
#include "InputEngine.hpp"  
#include "Lina/Input/InputAxisBinder.hpp"


namespace LinaEngine
{

	InputEngine::~InputEngine()
	{
		delete m_HorizontalAxis;
		delete m_VerticalAxis;

		LINA_CORE_TRACE("[Destructor] -> Input Engine ({0})", typeid(*this).name());
	}

	void InputEngine::Initialize()
	{
		LINA_CORE_TRACE("[Initialization] -> Input Engine ({0})", typeid(*this).name());

		m_HorizontalAxis = new InputKeyAxisBinder();
		m_VerticalAxis = new InputKeyAxisBinder();
		m_HorizontalAxis->Initialize(Input::Key::J, Input::Key::L);
		m_VerticalAxis->Initialize(Input::Key::I, Input::Key::K);
	}

	InputKeyAxisBinder* InputEngine::GetHorizontalInput()
	{
		return m_HorizontalAxis;
	}

	InputKeyAxisBinder* InputEngine::GetVerticalInput()
	{
		return m_VerticalAxis;
	}

	InputEngine::InputEngine()
	{
		LINA_CORE_TRACE("[Constructor] -> Input Engine ({0})", typeid(*this).name());
	}


}

