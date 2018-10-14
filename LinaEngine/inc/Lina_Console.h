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
9/30/2018 4:14:11 PM

*/

#pragma once

#ifndef Lina_Console_H
#define Lina_Console_H

#define CONSOLECOLOR_WHITE 15
#define CONSOLECOLOR_GREEN 10
#define CONSOLECOLOR_RED 12
#define CONSOLECOLOR_YELLOW 14
#define CONSOLECOLOR_CYAN 11
#define CONSOLECOLOR_PURPLE 13
#define CONSOLECOLOR_WHITEOVERRED 79
#define CONSOLECOLOR_WHITEOVERGREEN 47
#define CONSOLECOLOR_WHITEOVERGREY 143


static std::string lastSender;

class Lina_Console
{

public:

	enum MsgType { Debug, Success, Warning, Error, Initialization, Update, Deinitialization };


	// Returns the current time in string format. Pass in %I, %M, %S, %p %F for formatting.
	std::string now(const char* format = "%c")
	{
		std::time_t t = std::time(0);
		char cstr[128];
		std::strftime(cstr, sizeof(cstr), format, std::localtime(&t));
		return cstr;
	}

	// Adds a colored message to the console.
	inline void AddConsoleMsg(std::string msg, MsgType type, std::string sender)
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
		else if (type == MsgType::Initialization)
			colorVal = CONSOLECOLOR_WHITEOVERGREEN;
		else if (type == MsgType::Deinitialization)
			colorVal = CONSOLECOLOR_WHITEOVERRED;
		else if (type == MsgType::Update)
			colorVal = CONSOLECOLOR_WHITEOVERGREY;

		// Set color.
		SetConsoleTextAttribute(hConsole, colorVal);



		// print.
		std::cout << std::endl <<  "-> " << sender << " ->: " << msg << " T = " << now("%I:%M:%S %p")<< std::endl;


		// Set the console color back to white.
		colorVal = CONSOLECOLOR_WHITE;
		SetConsoleTextAttribute(hConsole, colorVal);
	}
	

	inline void AddConsoleMsgSameLine(std::string msg, MsgType type, std::string sender)
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
		else if (type == MsgType::Initialization)
			colorVal = CONSOLECOLOR_WHITEOVERGREEN;
		else if (type == MsgType::Deinitialization)
			colorVal = CONSOLECOLOR_WHITEOVERRED;
		else if (type == MsgType::Update)
			colorVal = CONSOLECOLOR_WHITEOVERGREY;

		// Set color.
		SetConsoleTextAttribute(hConsole, colorVal);

		// Set a new line if the sender is different.
		std::string bgn = lastSender == sender ? "" : "\n";
		lastSender = sender;
		// print.
		std::cout << bgn << "\r -> " << sender << " ->: " << msg << " T = " << now("%I:%M:%S %p") << std::flush;


		// Set the console color back to white.
		colorVal = CONSOLECOLOR_WHITE;
		SetConsoleTextAttribute(hConsole, colorVal);
	}

private:


};


#endif