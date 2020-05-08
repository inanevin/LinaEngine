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
10/26/2018 7:19:07 PM

*/

#pragma once

#ifndef Lina_Color_HPP
#define Lina_Color_HPP

#include "Core/Common.hpp"
#include "Interfaces/ISerializable.hpp"

namespace LinaEngine
{
	class  Color : public ISerializable
	{

	public:

		Color(float rv = 1.0f, float gv = 1.0f, float bv = 1.0f, float av = 1.0f) : r(rv), g(gv), b(bv), a(av) {};
		FORCEINLINE float R() { return r; }
		FORCEINLINE float G() { return g; }
		FORCEINLINE float B() { return b; }
		FORCEINLINE float A() { return a; }
		FORCEINLINE float R() const { return r; }
		FORCEINLINE float G() const { return g; }
		FORCEINLINE float B() const { return b; }
		FORCEINLINE float A() const { return a; }

		// Serialize the members.
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& r& g& b& a;
		}


		virtual void WriteObject(std::string& path) override
		{
			std::ofstream ofs(path);
			boost::archive::text_oarchive ar(ofs);
			ar& this;
		}

		virtual void ReadObject(std::string& path) override
		{
			std::ifstream ifs(path);
			boost::archive::text_iarchive ar(ifs);

			Color restoredData;
			ar& restoredData;

			// Set information.
			r = restoredData.r;
			g = restoredData.g;
			b = restoredData.b;
			a = restoredData.a;
		}

	private:

		// Allow serialization to access non-public data members.
		friend class boost::serialization::access;	

		float r, g, b, a = 1.0f;
	};

	namespace Colors
	{
		static Color Red = Color(1, 0, 0, 1);
		static Color Green = Color(0, 1, 0);
		static Color LightBlue = Color(0.4f, 0.4f, 0.8f);
		static Color Blue = Color(0, 0, 1);
		static Color DarkBlue = Color(0.1f, 0.1f, 0.6f);
		static Color Cyan = Color(0, 1, 1);
		static Color Yellow = Color(1, 1, 0);
		static Color Black = Color(0, 0, 0);
		static Color White = Color(1, 1, 1);
		static Color Purple = Color(1, 0, 1);
		static Color Maroon = Color(0.5f, 0, 0);
		static Color Beige = Color(0.96f, 0.96f, 0.862f);
		static Color Brown = Color(0.647f, 0.164f, 0.164f);
		static Color Gray = Color(0.5f, 0.5f, 0.5f);
	}

}


#endif