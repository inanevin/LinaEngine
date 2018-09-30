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
9/30/2018 4:14:11 PM

*/

#pragma once

#ifndef Lina_Console_H
#define Lina_Console_H

#define CONSOLECOLOR_WHITE 15
#define CONSOLECOLOR_GREEN 10
#define CONSOLECOLOR_RED 12
#define CONSOLECOLOR_YELLOW 14

class Lina_Console
{

public:

	enum MsgType {Debug, Success, Warning, Error};
	
	// Returns the current time in string format. Pass in %I, %M, %S, %p %F for formatting.
	std::string now(const char* format = "%c")
	{
		std::time_t t = std::time(0);
		char cstr[128];
		std::strftime(cstr, sizeof(cstr), format, std::localtime(&t));
		return cstr;
	}

	// Adds a colored message to the console.
	void AddConsoleMsg(std::string msg, MsgType type)
	{
		// Get the console.
		HANDLE hConsole;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		// Choose a color acc to the type.
		int colorVal = CONSOLECOLOR_WHITE;	// White.

		if (type == MsgType::Error)
			colorVal = CONSOLECOLOR_RED;
		else if (type == MsgType::Warning)
			colorVal = CONSOLECOLOR_YELLOW;
		else if (type == MsgType::Success)
			colorVal = CONSOLECOLOR_GREEN;

		// Set color.
		SetConsoleTextAttribute(hConsole, colorVal);

		// print.
		std::cout << "-> " << msg << " T = " << now("%I:%M:%S %p") << std::endl;

		// Set the console color back to white.
		colorVal = CONSOLECOLOR_WHITE;
		SetConsoleTextAttribute(hConsole, colorVal);
	}

};


#endif