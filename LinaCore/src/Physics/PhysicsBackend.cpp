/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Core/Physics/PhysicsBackend.hpp"
#include "Core/Physics/CommonPhysics.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <regex>

namespace
{
	// Callback for traces, connect this to your own trace function if you have one
	static void TraceImpl(const char* inFMT, ...)
	{
		va_list args;
		va_start(args, inFMT);

		// Use vsnprintf to safely format the string into a buffer.
		char buffer[4096]; // Adjust buffer size as needed.
		vsnprintf(buffer, sizeof(buffer), inFMT, args);

		// Convert buffer to std::string for regex replacement.
		std::string formattedMessage(buffer);

		// Escape curly braces by replacing `{` with `{{` and `}` with `}}`.
		formattedMessage = std::regex_replace(formattedMessage, std::regex("\\{"), "{{");
		formattedMessage = std::regex_replace(formattedMessage, std::regex("\\}"), "}}");

		// Pass the formatted and sanitized message to LINA_ERR.
		LINA_INFO(formattedMessage.c_str());

		va_end(args);
	}

#ifdef JPH_ENABLE_ASSERTS

	// Callback for asserts, connect this to your own assert handler if you have one
	static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint32 inLine)
	{
		LINA_ASSERT(false, inExpression);
		return true;
	};

#endif // JPH_ENABLE_ASSERTS

} // namespace
namespace Lina
{

	void PhysicsBackend::Initialize()
	{
		JPH::RegisterDefaultAllocator();
		JPH::Trace = TraceImpl;
		JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)
		JPH::Factory::sInstance = new JPH::Factory();
		JPH::RegisterTypes();
	}

	void PhysicsBackend::Shutdown()
	{
		JPH::UnregisterTypes();
		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}

} // namespace Lina
