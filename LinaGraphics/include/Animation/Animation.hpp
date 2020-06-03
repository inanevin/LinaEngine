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

Class: Animation
Timestamp: 6/3/2020 1:47:34 PM

*/
#pragma once

#ifndef Animation_HPP
#define Animation_HPP

#include "KeyFrame.hpp"


namespace LinaEngine::Graphics
{
	class Animation
	{
		
	public:
		
		Animation() {};
		Animation(float length, std::vector<KeyFrame> keyFrames) { m_Length = length; m_KeyFrames = keyFrames; }
		~Animation() {};
	
		FORCEINLINE float GetLength() { return m_Length; }
		FORCEINLINE std::vector<KeyFrame>& GetKeyFrames() { return m_KeyFrames; }

	private:
	
		float m_Length;
		std::vector<KeyFrame> m_KeyFrames;
	};
}

#endif
