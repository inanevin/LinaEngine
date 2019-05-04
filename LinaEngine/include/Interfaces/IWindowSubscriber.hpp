/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: IWindowSubscriber
Timestamp: 5/4/2019 3:05:36 PM

*/

#pragma once

#ifndef IWindowSubscriber_HPP
#define IWindowSubscriber_HPP

#include "Events/ActionSubscriber.hpp"


namespace LinaEngine 
{
	class IWindowSubscriber : public ActionSubscriber
	{
	public:

		IWindowSubscriber() {};
		virtual ~IWindowSubscriber() {};

	protected:

		void SubscribeWindowResizedAction(const LinaString& handlerID, std::function<void(LinaPair<float,float>)>&& cb)
		{
			SubscribeAction<LinaPair<float, float>>(handlerID, ActionType::WindowResized, cb);
		}
	};
}


#endif