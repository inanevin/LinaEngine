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
10/9/2018 4:44:45 AM

*/

#pragma once

#ifndef Lina_CoreMessageBus_H
#define Lina_CoreMessageBus_H

#include "Lina_InputEngine.h"
class Lina_ActionDispatcher;

class Lina_CoreMessageBus
{
public:

	static Lina_CoreMessageBus& Instance()
	{
		static Lina_CoreMessageBus instance; 				  
		return instance;
	}


	Lina_ActionDispatcher* GetInputDispatcher()
	{
		if (inputDispatcher == nullptr)
			throw "Pointer you are trying to get (inputDispatcher) does not point to a valid object!";

		return inputDispatcher;
	}

	Lina_InputEngine* GetInputEngine()
	{
		if(inputEngine == nullptr)
			throw "Pointer you are trying to get (inputDispatcher) does not point to a valid object!";

		return inputEngine;
	}

	void SetInputDispatcher(Lina_ActionDispatcher* disp) { inputDispatcher = disp; }
	void SetInputEngine(Lina_InputEngine* inp) { inputEngine = inp; }

private:
	Lina_CoreMessageBus() {}

	//Lina_CoreMessageBus(Lina_CoreMessageBus const&);  // Don't Implement
	//void operator=(Lina_CoreMessageBus const&); // Don't implement
	/* INSTANCES */
	Lina_InputEngine* inputEngine;
	Lina_ActionDispatcher* inputDispatcher;

public:

	Lina_CoreMessageBus(Lina_CoreMessageBus const&) = delete;
	void operator=(Lina_CoreMessageBus const&) = delete;

};


#endif