/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: UtilityFunctions
Timestamp: 1/5/2019 12:42:58 AM

*/

#ifndef UtilityFunctions_HPP
#define UtilityFunctions_HPP

#include <string>
#include <vector>
#include "Core/LinaAPI.hpp"
#include <functional>
namespace LinaEngine
{
	namespace Utility
	{
		// Gets a unique ID
		int GetUniqueID();

		// Gets a UniqueID as string
		std::string GetUniqueIDString();

		// Create a unique hash from string.
		size_t StringToHash(std::string str);

		// Split a string vector.
		std::vector<std::string> split(const std::string& s, char delim);

		// Return path via filename.
		std::string getFilePath(const std::string& fileName);

		// Loads a text file, parsing through include keywords.
		bool LoadTextFileWithIncludes(std::string& output, const std::string& fileName, const std::string& includeKeyword);

		static int uniqueID = 100;


		class LinaFunction 
		{
		private:

			// have a placeholder base-type:
			struct Base 
			{
				virtual ~Base();
			};

			// have a derived type to wrap the actual functor object:
			template <typename... Args>
			struct Wrapped : Base 
			{
				typedef std::function<void(Args...)> func_type;
				func_type f;
				Wrapped(func_type aFunc) : f(aFunc) { };
			};

			// Hold a base-type pointer:
			std::unique_ptr<Base> p_base;

		public:

			// Initialize object with any function:
			template <typename... Args>
			LinaFunction(std::function<void(Args...)> aFunc) : p_base(new Wrapped<Args...>(aFunc)) { };

			// Assign object with any function:
			template <typename... Args>
			LinaFunction& operator=(std::function<void(Args...)> aFunc) 
			{
				p_base = std::unique_ptr<Base>(new Wrapped<Args...>(aFunc));
				return *this;
			};

			// Call the function by casting the base
			//  pointer back to its derived type:
			template <typename... Args>
			void operator()(Args... args) const 
			{
				Wrapped<Args...>* p_wrapped = dynamic_cast<Wrapped<Args...>*>(p_base.get());

				if (p_wrapped) // <- if cast successful
					p_wrapped->f(args...);
				else
					throw std::runtime_error("Invalid arguments to function object call!");
			};
		};

	}
}

#endif