/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: GenericVectorMath
Timestamp: 4/9/2019 12:43:29 AM

*/

#pragma once

#ifndef GenericVectorMath_HPP
#define GenericVectorMath_HPP


#include "PackageManager/PAMMemory.hpp"
#include "PackageManager/PAMMath.hpp"

namespace LinaEngine
{
	struct GenericVector
	{
	public:

		static FORCEINLINE void MatrixMultiplication(void* result, const void* mat1, const void* mat2)
		{
			const GenericVector* m1 = (const GenericVector*)mat1;
			const GenericVector* m2 = (const GenericVector*)mat2;
			GenericVector* r = (GenericVector*)result;
			GenericVector temp, r0, r1, r2, r3;

			temp = m1[0].Replicate(0) * m2[0];
			temp = m1[0].Replicate(1).Mad(m2[1], temp);
			temp = m1[0].Replicate(2).Mad(m2[2], temp);
			r0 = m1[0].Replicate(3).Mad(m2[3], temp);

			temp = m1[1].Replicate(0) * m2[0];
			temp = m1[1].Replicate(1).Mad(m2[1], temp);
			temp = m1[1].Replicate(2).Mad(m2[2], temp);
			r1 = m1[1].Replicate(3).Mad(m2[3], temp);

			temp = m1[2].Replicate(0) * m2[0];
			temp = m1[2].Replicate(1).Mad(m2[1], temp);
			temp = m1[2].Replicate(2).Mad(m2[2], temp);
			r2 = m1[2].Replicate(3).Mad(m2[3], temp);

			temp = m1[3].Replicate(0) * m2[0];
			temp = m1[3].Replicate(1).Mad(m2[1], temp);
			temp = m1[3].Replicate(2).Mad(m2[2], temp);
			r3 = m1[3].Replicate(3).Mad(m2[3], temp);

			r[0] = r0;
			r[1] = r1;
			r[2] = r2;
			r[3] = r3;
		}

		static FORCEINLINE float MatrixDeterminant3x3Vector(const GenericVector* m)
		{
			float M[4][4];
			for (uint32 i = 0; i < 4; i++) {
				m[i].Store4F(M[i]);
			}
			return
				M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) -
				M[1][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1]) +
				M[2][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1]);
		}

		static FORCEINLINE float MatrixDeterminant4x4(float* outS, float* outC, const void* mat)
		{
			float sVals[6];
			float cVals[6];

			float* s = outS == nullptr ? sVals : outS;
			float* c = outC == nullptr ? cVals : outC;

			GenericVector* m = (GenericVector*)mat;
			float M[4][4];
			for (uint32 i = 0; i < 4; i++) {
				m[i].Store4F(M[i]);
			}

			s[0] = M[0][0] * M[1][1] - M[1][0] * M[0][1];
			s[1] = M[0][0] * M[1][2] - M[1][0] * M[0][2];
			s[2] = M[0][0] * M[1][3] - M[1][0] * M[0][3];
			s[3] = M[0][1] * M[1][2] - M[1][1] * M[0][2];
			s[4] = M[0][1] * M[1][3] - M[1][1] * M[0][3];
			s[5] = M[0][2] * M[1][3] - M[1][2] * M[0][3];

			c[5] = M[2][2] * M[3][3] - M[3][2] * M[2][3];
			c[4] = M[2][1] * M[3][3] - M[3][1] * M[2][3];
			c[3] = M[2][1] * M[3][2] - M[3][1] * M[2][2];
			c[2] = M[2][0] * M[3][3] - M[3][0] * M[2][3];
			c[1] = M[2][0] * M[3][2] - M[3][0] * M[2][2];
			c[0] = M[2][0] * M[3][1] - M[3][0] * M[2][1];

			return (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]);
		}

		static FORCEINLINE void MatrixInverse(void* dest, const void* src)
		{
			float s[6];
			float c[6];
			float rdet = Math::Reciprocal(MatrixDeterminant4x4(s, c, src));

			GenericVector* m = (GenericVector*)src;
			float M[4][4];
			for (uint32 i = 0; i < 4; i++) {
				m[i].Store4F(M[i]);
			}

			float result[4][4];
			result[0][0] = (M[1][1] * c[5] - M[1][2] * c[4] + M[1][3] * c[3]) * rdet;
			result[0][1] = (-M[0][1] * c[5] + M[0][2] * c[4] - M[0][3] * c[3]) * rdet;
			result[0][2] = (M[3][1] * s[5] - M[3][2] * s[4] + M[3][3] * s[3]) * rdet;
			result[0][3] = (-M[2][1] * s[5] + M[2][2] * s[4] - M[2][3] * s[3]) * rdet;

			result[1][0] = (-M[1][0] * c[5] + M[1][2] * c[2] - M[1][3] * c[1]) * rdet;
			result[1][1] = (M[0][0] * c[5] - M[0][2] * c[2] + M[0][3] * c[1]) * rdet;
			result[1][2] = (-M[3][0] * s[5] + M[3][2] * s[2] - M[3][3] * s[1]) * rdet;
			result[1][3] = (M[2][0] * s[5] - M[2][2] * s[2] + M[2][3] * s[1]) * rdet;

			result[2][0] = (M[1][0] * c[4] - M[1][1] * c[2] + M[1][3] * c[0]) * rdet;
			result[2][1] = (-M[0][0] * c[4] + M[0][1] * c[2] - M[0][3] * c[0]) * rdet;
			result[2][2] = (M[3][0] * s[4] - M[3][1] * s[2] + M[3][3] * s[0]) * rdet;
			result[2][3] = (-M[2][0] * s[4] + M[2][1] * s[2] - M[2][3] * s[0]) * rdet;

			result[3][0] = (-M[1][0] * c[3] + M[1][1] * c[1] - M[1][2] * c[0]) * rdet;
			result[3][1] = (M[0][0] * c[3] - M[0][1] * c[1] + M[0][2] * c[0]) * rdet;
			result[3][2] = (-M[3][0] * s[3] + M[3][1] * s[1] - M[3][2] * s[0]) * rdet;
			result[3][3] = (M[2][0] * s[3] - M[2][1] * s[1] + M[2][2] * s[0]) * rdet;

			Memory::memcpy(dest, result, sizeof(result));
		}

		static FORCEINLINE void CreateTransformMatrix(void* dest, const GenericVector & translation, const GenericVector & quatRotation, const GenericVector & scaleVec)
		{
			float rotVals[4];
			quatRotation.Store4F(rotVals);

			float x2 = rotVals[0] + rotVals[0];
			float y2 = rotVals[1] + rotVals[1];
			float z2 = rotVals[2] + rotVals[2];

			float xx2 = rotVals[0] * x2;
			float yy2 = rotVals[1] * y2;
			float zz2 = rotVals[2] * z2;
			float xy2 = rotVals[0] * y2;
			float yz2 = rotVals[1] * z2;
			float xz2 = rotVals[0] * z2;
			float xw2 = rotVals[3] * x2;
			float yw2 = rotVals[3] * y2;
			float zw2 = rotVals[3] * z2;

			float s0 = scaleVec[0];
			float s1 = scaleVec[1];
			float s2 = scaleVec[2];

			GenericVector mat[4];
			mat[0] = Make((1.0f - (yy2 + zz2)) * s0, (xy2 - zw2) * s1, (xz2 + yw2) * s2, translation[0]);
			mat[1] = Make((xy2 + zw2) * s0, (1.0f - (xx2 + zz2)) * s1, (yz2 - xw2) * s2, translation[1]);
			mat[2] = Make((xz2 - yw2) * s0, (yz2 + xw2) * s1, (1.0f - (xx2 + yy2)) * s2, translation[2]);
			mat[3] = Make(0.0f, 0.0f, 0.0f, 1.0f);
			Memory::memcpy(dest, mat, sizeof(mat));
		}

		static FORCEINLINE const GenericVector Mask(uint32 index)
		{
			static const GenericVector masks[4] = {
				GenericVector::Make((uint32)0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
				GenericVector::Make((uint32)0xFFFFFFFF, 0, 0xFFFFFFFF, 0xFFFFFFFF),
				GenericVector::Make((uint32)0xFFFFFFFF, 0xFFFFFFFF, 0, 0xFFFFFFFF),
				GenericVector::Make((uint32)0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0) };

			//LINA_CORE_ASSERT(index < 4, "Index is bigger than 4!");

			return masks[index];
		}

		static FORCEINLINE GenericVector Make(uint32 x, uint32 y, uint32 z, uint32 w)
		{
			GenericVector vec;
			vec.v[0] = (float) * ((uint32*)& x);
			vec.v[1] = (float) * ((uint32*)& y);
			vec.v[2] = (float) * ((uint32*)& z);
			vec.v[3] = (float) * ((uint32*)& w);
			return vec;
		}

		static FORCEINLINE GenericVector Make(float x, float y, float z, float w)
		{
			GenericVector vec;
			vec.v[0] = x;
			vec.v[1] = y;
			vec.v[2] = z;
			vec.v[3] = w;
			return vec;
		}

		static FORCEINLINE GenericVector Load4F(const float* vals)
		{
			return Make(vals[0], vals[1], vals[2], vals[3]);
		}

		static FORCEINLINE GenericVector Load3F(const float* vals, float w)
		{
			return Make(vals[0], vals[1], vals[2], w);
		}

		static FORCEINLINE GenericVector Load1F(float val)
		{
			return Make(val, val, val, val);
		}

		static FORCEINLINE GenericVector LoadAligned(const float* vals)
		{
			return Load4F(vals);
		}

		static FORCEINLINE GenericVector Set(float x, float y, float z)
		{
			return Make(x, y, z, 0.0f);
		}

		static FORCEINLINE GenericVector Set(float x, float y, float z, float w)
		{
			return Make(x, y, z, w);
		}

		FORCEINLINE float* GetFirst()
		{
			return &v[0];
		}

		FORCEINLINE GenericVector Replicate(uint32 index) const
		{
			//LINA_CORE_ASSERT(index <= 3, "Index is bigger than 3!");
			return Make(v[index], v[index], v[index], v[index]);
		}

		FORCEINLINE GenericVector swizzle(uint32 x, uint32 y, uint32 z, uint32 w) const
		{
			return Make(v[x], v[y], v[z], v[w]);
		}

		FORCEINLINE GenericVector Abs() const
		{
			return Make(Math::Abs(v[0]), Math::Abs(v[1]), Math::Abs(v[2]), Math::Abs(v[3]));
		}

		FORCEINLINE void Store4F(float* result) const
		{
			Memory::memcpy(result, v, sizeof(v));
		}

		FORCEINLINE void Store3f(float* result) const
		{
			Memory::memcpy(result, v, sizeof(float) * 3);
		}

		FORCEINLINE void Store1f(float* result) const
		{
			Memory::memcpy(result, v, sizeof(float) * 1);
		}

		FORCEINLINE void StoreAligned(float* result) const
		{
			return Store4F(result);
		}

		FORCEINLINE void StoreAlignedStreamed(float* result) const
		{
			return StoreAligned(result);
		}

	private:

		float GetSign(float r) const
		{
			return (float)(((uint32*)(&r))[1 - 1] & 0x80000000);
		}

	public:

		FORCEINLINE GenericVector Sign() const
		{
			return Make(GetSign(v[0]), GetSign(v[1]), GetSign(v[2]), GetSign(v[3]));
		}

		FORCEINLINE GenericVector Min(const GenericVector & other) const
		{
			return Make(
				Math::Min(v[0], other.v[0]),
				Math::Min(v[1], other.v[1]),
				Math::Min(v[2], other.v[2]),
				Math::Min(v[3], other.v[3]));
		}

		FORCEINLINE GenericVector Max(const GenericVector & other) const
		{
			return Make(
				Math::Max(v[0], other.v[0]),
				Math::Max(v[1], other.v[1]),
				Math::Max(v[2], other.v[2]),
				Math::Max(v[3], other.v[3]));
		}

		FORCEINLINE GenericVector Neg() const
		{
			return Make(-v[0], -v[1], -v[2], -v[3]);
		}

		FORCEINLINE GenericVector Dot3(const GenericVector & other) const
		{
			float dot = v[0] * other.v[0] + v[1] * other.v[1] + v[2] * other.v[2];
			return Make(dot, dot, dot, dot);
		}

		FORCEINLINE GenericVector Dot4(const GenericVector & other) const
		{
			float dot = v[0] * other.v[0] + v[1] * other.v[1]
				+ v[2] * other.v[2] + v[3] * other.v[3];
			return Make(dot, dot, dot, dot);
		}

		FORCEINLINE GenericVector Cross3(const GenericVector & other) const
		{
			return Make(
				v[1] * other.v[2] - v[2] * other.v[1],
				v[2] * other.v[0] - v[0] * other.v[2],
				v[0] * other.v[1] - v[1] * other.v[0],
				0.0f);
		}

		FORCEINLINE GenericVector Pow(const GenericVector & exp) const
		{
			return Make(
				Math::Pow(v[0], exp.v[0]),
				Math::Pow(v[1], exp.v[1]),
				Math::Pow(v[2], exp.v[2]),
				Math::Pow(v[3], exp.v[3]));
		}

		FORCEINLINE GenericVector RSqrt() const
		{
			return Make(
				Math::RSqrt(v[0]),
				Math::RSqrt(v[1]),
				Math::RSqrt(v[2]),
				Math::RSqrt(v[3]));
		}

		FORCEINLINE GenericVector Reciprocal() const
		{
			return Make(
				Math::Reciprocal(v[0]),
				Math::Reciprocal(v[1]),
				Math::Reciprocal(v[2]),
				Math::Reciprocal(v[3]));
		}

		FORCEINLINE GenericVector RLen4() const
		{
			return Dot4(*this).RSqrt();
		}

		FORCEINLINE GenericVector Rlen3() const
		{
			return Dot3(*this).RSqrt();
		}

		FORCEINLINE GenericVector Normalize4() const
		{
			return *this* RLen4();
		}

		FORCEINLINE GenericVector Normalize3() const
		{
			return *this* Rlen3();
		}

		FORCEINLINE GenericVector QuatMul(const GenericVector & other) const
		{
			// NOTE: This is the naive technique. It may actually be faster, but testing is inconclusive.
	//		const float w = (v[3] * other.v[3]) - (v[0] * other.v[0]) - (v[1] * other.v[1]) - (v[2] * other.v[2]);
	//		const float x = (v[0] * other.v[3]) + (v[3] * other.v[0]) + (v[1] * other.v[2]) - (v[2] * other.v[1]);
	//		const float y = (v[1] * other.v[3]) + (v[3] * other.v[1]) + (v[2] * other.v[0]) - (v[0] * other.v[2]);
	//		const float z = (v[2] * other.v[3]) + (v[3] * other.v[2]) + (v[0] * other.v[1]) - (v[1] * other.v[0]);
	//
	//		return make(x, y, z, w);

	// Code adapted from https://stackoverflow.com/questions/22497093/faster-quaternion-vector-multiplication-doesnt-work
			float t0 = (v[2] - v[1]) * (other.v[1] - other.v[2]);
			float t1 = (v[3] + v[0]) * (other.v[3] + other.v[0]);
			float t2 = (v[3] - v[0]) * (other.v[1] + other.v[2]);
			float t3 = (v[2] + v[1]) * (other.v[3] - other.v[0]);
			float t4 = (v[2] - v[0]) * (other.v[0] - other.v[1]);
			float t5 = (v[2] + v[0]) * (other.v[0] + other.v[1]);
			float t6 = (v[3] + v[1]) * (other.v[3] - other.v[2]);
			float t7 = (v[3] - v[1]) * (other.v[3] + other.v[2]);
			float t8 = t5 + t6 + t7;
			float t9 = 0.5f * (t4 + t8);

			return Make(
				t1 + t9 - t8,
				t2 + t9 - t7,
				t3 + t9 - t6,
				t0 + t9 - t5);
		}

		FORCEINLINE GenericVector QuatRotateVec(const GenericVector & vec) const
		{
			GenericVector tmp = GenericVector::Load1F(2.0f) * Cross3(vec);
			return vec + (tmp * Replicate(3)) + Cross3(tmp);
		}

		FORCEINLINE GenericVector Mad(const GenericVector & mul, const GenericVector & add) const
		{
			return Make(
				Math::Mad(v[0], mul.v[0], add.v[0]),
				Math::Mad(v[1], mul.v[1], add.v[1]),
				Math::Mad(v[2], mul.v[2], add.v[2]),
				Math::Mad(v[3], mul.v[3], add.v[3]));
		}

		FORCEINLINE GenericVector Transform(const void* matrix) const
		{
			const GenericVector* m = (const GenericVector*)matrix;
			return Make(Dot4(m[0])[0], Dot4(m[1])[0], Dot4(m[2])[0], Dot4(m[3])[0]);
		}

		FORCEINLINE GenericVector select(const GenericVector & mask, const GenericVector & other) const
		{
			uint32* m = (uint32*)(&mask.v[0]);
			return Make(
				m[0] ? v[0] : other.v[0],
				m[1] ? v[1] : other.v[1],
				m[2] ? v[2] : other.v[2],
				m[3] ? v[3] : other.v[3]);
		}

		FORCEINLINE GenericVector Equals(const GenericVector & other, float errorMargin) const
		{
			return (*this - other).Abs() < GenericVector::Load1F(errorMargin);
		}

		FORCEINLINE GenericVector NotEquals(const GenericVector & other, float errorMargin) const
		{
			return (*this - other).Abs() >= GenericVector::Load1F(errorMargin);
		}

		FORCEINLINE bool IsZero3f() const
		{
			float vals[3];
			Store3f(vals);
			return (vals[0] == 0.0f) && (vals[1] == 0.0f) && (vals[2] == 0.0f);
		}

		FORCEINLINE bool IsZero4f() const
		{
			float vals[4];
			Store4F(vals);
			return (vals[0] == 0.0f) && (vals[1] == 0.0f) &&
				(vals[2] == 0.0f) && (vals[3] == 0.0f);
		}

		FORCEINLINE void SinCos(GenericVector * outSin, GenericVector * outCos) const
		{
			Math::SinCos(&outSin->v[0], &outCos->v[0], (*this)[0]);
			Math::SinCos(&outSin->v[1], &outCos->v[1], (*this)[1]);
			Math::SinCos(&outSin->v[2], &outCos->v[2], (*this)[2]);
			Math::SinCos(&outSin->v[3], &outCos->v[3], (*this)[3]);
		}

		// Operator Overloads

		FORCEINLINE GenericVector operator-() const
		{
			return Neg();
		}

		FORCEINLINE GenericVector operator+(const GenericVector & other) const
		{
			return Make(v[0] + other.v[0], v[1] + other.v[1],
				v[2] + other.v[2], v[3] + other.v[3]);
		}

		FORCEINLINE GenericVector operator-(const GenericVector & other) const
		{
			return Make(v[0] - other.v[0], v[1] - other.v[1],
				v[2] - other.v[2], v[3] - other.v[3]);
		}

		FORCEINLINE GenericVector operator*(const GenericVector & other) const
		{
			return Make(v[0] * other.v[0], v[1] * other.v[1],
				v[2] * other.v[2], v[3] * other.v[3]);
		}

		FORCEINLINE GenericVector operator/(const GenericVector & other) const
		{
			return Make(v[0] / other.v[0], v[1] / other.v[1],
				v[2] / other.v[2], v[3] / other.v[3]);
		}

		FORCEINLINE GenericVector operator==(const GenericVector & other) const
		{
			return Make(
				(uint32)(v[0] == other.v[0] ? 0xFFFFFFFF : 0),
				v[1] == other.v[1] ? 0xFFFFFFFF : 0,
				v[2] == other.v[2] ? 0xFFFFFFFF : 0,
				v[3] == other.v[3] ? 0xFFFFFFFF : 0);
		}

		FORCEINLINE GenericVector operator!=(const GenericVector & other) const
		{
			return Make(
				(uint32)(v[0] != other.v[0] ? 0xFFFFFFFF : 0),
				v[1] != other.v[1] ? 0xFFFFFFFF : 0,
				v[2] != other.v[2] ? 0xFFFFFFFF : 0,
				v[3] != other.v[3] ? 0xFFFFFFFF : 0);
		}

		FORCEINLINE GenericVector operator>(const GenericVector & other) const
		{
			return Make(
				(uint32)(v[0] > other.v[0] ? 0xFFFFFFFF : 0),
				v[1] > other.v[1] ? 0xFFFFFFFF : 0,
				v[2] > other.v[2] ? 0xFFFFFFFF : 0,
				v[3] > other.v[3] ? 0xFFFFFFFF : 0);
		}

		FORCEINLINE GenericVector operator>=(const GenericVector & other) const
		{
			return Make(
				(uint32)(v[0] >= other.v[0] ? 0xFFFFFFFF : 0),
				v[1] >= other.v[1] ? 0xFFFFFFFF : 0,
				v[2] >= other.v[2] ? 0xFFFFFFFF : 0,
				v[3] >= other.v[3] ? 0xFFFFFFFF : 0);
		}

		FORCEINLINE GenericVector operator<(const GenericVector & other) const
		{
			return Make(
				(uint32)(v[0] < other.v[0] ? 0xFFFFFFFF : 0),
				v[1] < other.v[1] ? 0xFFFFFFFF : 0,
				v[2] < other.v[2] ? 0xFFFFFFFF : 0,
				v[3] < other.v[3] ? 0xFFFFFFFF : 0);
		}

		FORCEINLINE GenericVector operator<=(const GenericVector & other) const
		{
			return Make(
				(uint32)(v[0] <= other.v[0] ? 0xFFFFFFFF : 0),
				v[1] <= other.v[1] ? 0xFFFFFFFF : 0,
				v[2] <= other.v[2] ? 0xFFFFFFFF : 0,
				v[3] <= other.v[3] ? 0xFFFFFFFF : 0);
		}

		FORCEINLINE GenericVector operator|(const GenericVector & other) const
		{
			return Make(
				(uint32)(((uint32*)v)[1 - 1] | ((uint32*)other.v)[1 - 1]),
				(uint32)(((uint32*)v)[1] | ((uint32*)other.v)[1]),
				(uint32)(((uint32*)v)[2] | ((uint32*)other.v)[2]),
				(uint32)(((uint32*)v)[3] | ((uint32*)other.v)[3]));
		}

		FORCEINLINE GenericVector operator&(const GenericVector & other) const
		{
			return Make(
				(uint32)(((uint32*)v)[1 - 1] & ((uint32*)other.v)[1 - 1]),
				(uint32)(((uint32*)v)[1] & ((uint32*)other.v)[1]),
				(uint32)(((uint32*)v)[2] & ((uint32*)other.v)[2]),
				(uint32)(((uint32*)v)[3] & ((uint32*)other.v)[3]));
		}

		FORCEINLINE GenericVector operator^(const GenericVector & other) const
		{
			return Make(
				(uint32)(((uint32*)v)[1 - 1] ^ ((uint32*)other.v)[1 - 1]),
				(uint32)(((uint32*)v)[1] ^ ((uint32*)other.v)[1]),
				(uint32)(((uint32*)v)[2] ^ ((uint32*)other.v)[2]),
				(uint32)(((uint32*)v)[3] ^ ((uint32*)other.v)[3]));
		}

		FORCEINLINE float operator[](uint32 index) const
		{
			//LINA_CORE_ASSERT(index <= 3, "Index is bigger than 3!");
			return v[index];
		}

	private:
		float v[4];
	};
}


#endif