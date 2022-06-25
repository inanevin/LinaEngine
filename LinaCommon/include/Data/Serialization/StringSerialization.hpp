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



#pragma once

#ifndef DataStructuresStringSerialization_HPP
#define DataStructuresStringSerialization_HPP

#include <EASTL/string.h>

namespace cereal
{
   //! Serialization for basic_string types, if binary data is supported
   template<class Archive, class CharT, class Traits> inline
       typename std::enable_if<traits::is_output_serializable<BinaryData<CharT>, Archive>::value, void>::type
       CEREAL_SAVE_FUNCTION_NAME(Archive& ar, eastl::basic_string<CharT, Traits> const& str)
   {
       // Save number of chars + the data
       ar(make_size_tag(static_cast<size_type>(str.size())));
       ar(binary_data(str.data(), str.size() * sizeof(CharT)));
   }
  
   //! Serialization for basic_string types, if binary data is supported
   template<class Archive, class CharT, class Traits> inline
       typename std::enable_if<traits::is_input_serializable<BinaryData<CharT>, Archive>::value, void>::type
       CEREAL_LOAD_FUNCTION_NAME(Archive& ar, eastl::basic_string<CharT, Traits>& str)
   {
       size_type size;
       ar(make_size_tag(size));
       str.resize(static_cast<std::size_t>(size));
       ar(binary_data(const_cast<CharT*>(str.data()), static_cast<std::size_t>(size) * sizeof(CharT)));
   }
}  
#endif
