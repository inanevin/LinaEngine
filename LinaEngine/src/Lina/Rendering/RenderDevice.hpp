/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: RenderDevice
Timestamp: 4/15/2019 12:06:15 PM

*/

#pragma once

#ifndef RenderDevice_HPP
#define RenderDevice_HPP



namespace LinaEngine
{
	template<class Derived>
	class RenderDevice
	{
	public:

		FORCEINLINE RenderDevice() 
		{
			LINA_CORE_TRACE("[Constructor] -> RenderDevice ({0})", typeid(*this).name());
			m_Derived = static_cast<Derived*>(this);
		};

		virtual ~RenderDevice()
		{
			LINA_CORE_TRACE("[Destructor] -> RenderDevice ({0})", typeid(*this).name());
		}

		
	private:

		Derived* m_Derived;

	};
}


#endif