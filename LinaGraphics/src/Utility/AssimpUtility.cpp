/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Utility/AssimpUtility.hpp"

namespace Lina::Graphics
{
	Lina::Matrix AssimpToLinaMatrix(const aiMatrix4x4& aiMat)
	{
		Matrix mat;
		mat[0][0] = aiMat.a1;	mat[0][1] = aiMat.b1;	mat[0][2] = aiMat.c1;	mat[0][3] = aiMat.d1;
		mat[1][0] = aiMat.a2;	mat[1][1] = aiMat.b2;	mat[1][2] = aiMat.c2;	mat[1][3] = aiMat.d2;
		mat[2][0] = aiMat.a3;	mat[2][1] = aiMat.b3;	mat[2][2] = aiMat.c3;	mat[2][3] = aiMat.d3;
		mat[3][0] = aiMat.a4;	mat[3][1] = aiMat.b4;	mat[3][2] = aiMat.c4;	mat[3][3] = aiMat.d4;
		return mat;
	}

	Lina::Vector3 AssimpToLinaVector3(const aiVector3D& vec)
	{
		return Lina::Vector3(vec.x, vec.y, vec.z);
	}
}