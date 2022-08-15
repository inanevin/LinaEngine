/*
Class: ProgressPanel



Timestamp: 1/4/2022 5:33:09 PM
*/

#pragma once

#ifndef ProgressPanel_HPP
#define ProgressPanel_HPP

// Headers here.
#include <Data/String.hpp>

namespace Lina::Editor
{
	class ProgressPanel
	{
		
	public:
		
		ProgressPanel() = default;
		~ProgressPanel() = default;

		void Draw(const String& currentResource, float percentage);
	
	private:
	
	};
}

#endif
