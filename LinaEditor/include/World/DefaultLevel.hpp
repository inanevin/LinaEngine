/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: DefaultLevel
Timestamp: 10/16/2020 12:58:01 AM

*/
#pragma once

#ifndef DefaultLevel_HPP
#define DefaultLevel_HPP

// Headers here.
#include "World/Level.hpp"

namespace LinaEditor
{
	class DefaultLevel : public LinaEngine::World::Level
	{
		
	public:
		
		DefaultLevel() {};
		~DefaultLevel() {};
	
	private:
	

		// Inherited via Level
		virtual bool Install() override;
		virtual void Initialize() override;

	};
}

#endif
