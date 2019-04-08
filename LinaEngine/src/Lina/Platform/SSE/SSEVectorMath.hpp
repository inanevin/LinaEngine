/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: SSEVectorMath
Timestamp: 4/9/2019 12:33:52 AM

*/

#pragma once

#ifndef SSEVectorMath_HPP
#define SSEVectorMath_HPP

#include "Lina/Utility/Math/Math.hpp"
#include "Lina/Platform/PlatformSIMD.hpp"
#include "Lina/Memory.hpp"
#include "Lina/Core.hpp"

#define SSEVector_SHUFFLEMASK(a0,a1,b2,b3) ((a0) | ((a1)<<2) | ((b2)<<4) | ((b3)<<6))
#define SSEVector_Swizzle_0101(vec)               _mm_movelh_ps(vec, vec)
#define SSEVector_Swizzle_2323(vec)               _mm_movehl_ps(vec, vec)
#define SSEVector_Swizzle_0022(vec)               _mm_moveldup_ps(vec)
#define SSEVector_Swizzle_1133(vec)               _mm_movehdup_ps(vec)
#define SSEVector_Shuffle(vec1, vec2, x,y,z,w)    _mm_shuffle_ps(vec1, vec2, SSEVector_SHUFFLEMASK(x,y,z,w))
#define SSEVector_Swizzle(vec, x,y,z,w)           _mm_shuffle_ps(vec, vec, SSEVector_SHUFFLEMASK(x,y,z,w))
#define SSEVector_Shuffle_0101(vec1, vec2)        _mm_movelh_ps(vec1, vec2)
#define SSEVector_Shuffle_2323(vec1, vec2)        _mm_movehl_ps(vec2, vec1)

namespace LinaEngine
{
	struct SSEVector
	{
	public:
		static FORCEINLINE void MatrixMultiplication(void* result, const void* mat1, const void* mat2)
		{
			const SSEVector* m1 = (const SSEVector*)mat1;
			const SSEVector* m2 = (const SSEVector*)mat2;
			SSEVector* r = (SSEVector*)result;
			SSEVector temp, r0, r1, r2, r3;

			temp = m1[0].replicate(0) * m2[0];
			temp = m1[0].replicate(1).mad(m2[1], temp);
			temp = m1[0].replicate(2).mad(m2[2], temp);
			r0 = m1[0].replicate(3).mad(m2[3], temp);

			temp = m1[1].replicate(0) * m2[0];
			temp = m1[1].replicate(1).mad(m2[1], temp);
			temp = m1[1].replicate(2).mad(m2[2], temp);
			r1 = m1[1].replicate(3).mad(m2[3], temp);

			temp = m1[2].replicate(0) * m2[0];
			temp = m1[2].replicate(1).mad(m2[1], temp);
			temp = m1[2].replicate(2).mad(m2[2], temp);
			r2 = m1[2].replicate(3).mad(m2[3], temp);

			temp = m1[3].replicate(0) * m2[0];
			temp = m1[3].replicate(1).mad(m2[1], temp);
			temp = m1[3].replicate(2).mad(m2[2], temp);
			r3 = m1[3].replicate(3).mad(m2[3], temp);

			r[0] = r0;
			r[1] = r1;
			r[2] = r2;
			r[3] = r3;
		}

		static FORCEINLINE float MatrixDeterminant3x3Vector(const SSEVector* m)
		{
			float M[4][4];
			for (uint32 i = 0; i < 4; i++) {
				m[i].store4f(M[i]);
			}
			return
				M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) -
				M[1][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1]) +
				M[2][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1]);
		}

		static FORCEINLINE float matrixDeterminant4x4(float* outS,
			float* outC, const void* mat)
		{
			float sVals[6];
			float cVals[6];

			float* s = outS == nullptr ? sVals : outS;
			float* c = outC == nullptr ? cVals : outC;

			SSEVector* m = (SSEVector*)mat;
			float M[4][4];
			for (uint32 i = 0; i < 4; i++) {
				m[i].store4f(M[i]);
			}

			s[0] = M[0][0] * M[1][1] - M[1][0] * M[0][1];
			s[1] = M[0][0] * M[1][2] - M[1][0] * M[0][2];
			s[2] = M[0][0] * M[1][3] - M[1][0] * M[0][3];
			s[3] = M[0][1] * m[1][2] - M[1][1] * M[0][2];
			s[4] = M[0][1] * M[1][3] - M[1][1] * M[0][3];
			s[5] = M[0][2] * M[1][3] - M[1][2] * M[0][3];

			c[0] = M[2][0] * M[3][1] - M[3][0] * M[2][1];
			c[1] = M[2][0] * M[3][2] - M[3][0] * M[2][2];
			c[2] = M[2][0] * M[3][3] - M[3][0] * M[2][3];
			c[3] = M[2][1] * M[3][2] - M[3][1] * M[2][2];
			c[4] = M[2][1] * M[3][3] - M[3][1] * M[2][3];
			c[5] = M[2][2] * M[3][3] - M[3][2] * M[2][3];

			return (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]);
		}

		// Based on https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html
		static FORCEINLINE void matrixInverse(void* dest, const void* src)
		{
			__m128* mVec = (__m128*)src;
			float* m = (float*)src;
			__m128 A = SSEVector_Shuffle_0101(mVec[0], mVec[1]);
			__m128 B = SSEVector_Shuffle_2323(mVec[0], mVec[1]);
			__m128 C = SSEVector_Shuffle_0101(mVec[2], mVec[3]);
			__m128 D = SSEVector_Shuffle_2323(mVec[2], mVec[3]);

			__m128 detA = _mm_set1_ps(m[0 * 4 + 0] * m[1 * 4 + 1] - m[0 * 4 + 1] * m[1 * 4 + 0]);
			__m128 detB = _mm_set1_ps(m[0 * 4 + 2] * m[1 * 4 + 3] - m[0 * 4 + 3] * m[1 * 4 + 2]);
			__m128 detC = _mm_set1_ps(m[2 * 4 + 0] * m[3 * 4 + 1] - m[2 * 4 + 1] * m[3 * 4 + 0]);
			__m128 detD = _mm_set1_ps(m[2 * 4 + 2] * m[3 * 4 + 3] - m[2 * 4 + 3] * m[3 * 4 + 2]);

			__m128 D_C = mat2AdjMul(D, C);
			__m128 A_B = mat2AdjMul(A, B);
			__m128 X_ = _mm_sub_ps(_mm_mul_ps(detD, A), mat2Mul(B, D_C));
			__m128 W_ = _mm_sub_ps(_mm_mul_ps(detA, D), mat2Mul(C, A_B));

			__m128 detM = _mm_mul_ps(detA, detD);
			__m128 Y_ = _mm_sub_ps(_mm_mul_ps(detB, C), mat2MulAdj(D, A_B));
			__m128 Z_ = _mm_sub_ps(_mm_mul_ps(detC, B), mat2MulAdj(A, D_C));
			detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

			__m128 tr = horizontalAdd(_mm_mul_ps(A_B, SSEVector_Swizzle(D_C, 0, 2, 1, 3)));
			detM = _mm_sub_ps(detM, tr);

			const __m128 adjSignMask = _mm_setr_ps(1.f, -1.f, -1.f, 1.f);
			__m128 rDetM = _mm_div_ps(adjSignMask, detM);

			X_ = _mm_mul_ps(X_, rDetM);
			Y_ = _mm_mul_ps(Y_, rDetM);
			Z_ = _mm_mul_ps(Z_, rDetM);
			W_ = _mm_mul_ps(W_, rDetM);

			__m128* rmVec = (__m128*)dest;
			rmVec[0] = SSEVector_Shuffle(X_, Y_, 3, 1, 3, 1);
			rmVec[1] = SSEVector_Shuffle(X_, Y_, 2, 0, 2, 0);
			rmVec[2] = SSEVector_Shuffle(Z_, W_, 3, 1, 3, 1);
			rmVec[3] = SSEVector_Shuffle(Z_, W_, 2, 0, 2, 0);
		}

		static FORCEINLINE void createTransformMatrix(void* dest, const SSEVector& translation, const SSEVector& quatRotation, const SSEVector& scaleVec)
		{
			// NOTE: This can be further vectorized!
			// NOTE: This may actually be faster without vectorization, but testing is inconclusive.
			static const SSEVector MASK_W(SSEVector::make((uint32)0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0));
			SSEVector rot2 = quatRotation + quatRotation;
			SSEVector rs12 = quatRotation * rot2;
			SSEVector rs22;
			rs22.data = _mm_mul_ps(
				SSEVector_Swizzle(quatRotation.data, 0, 1, 0, 3),
				SSEVector_Swizzle(rot2.data, 1, 2, 2, 3));
			SSEVector rs32 = quatRotation.replicate(3)*rot2;

			float xx2 = rs12[0];
			float yy2 = rs12[1];
			float zz2 = rs12[2];
			float xy2 = rs22[0];
			float yz2 = rs22[1];
			float xz2 = rs22[2];
			float xw2 = rs32[0];
			float yw2 = rs32[1];
			float zw2 = rs32[2];

			SSEVector newScale = scaleVec.select(MASK_W, SSEVector::load1f(1.0f));
			SSEVector* mat = (SSEVector*)dest;
			mat[0] = newScale * make((1.0f - (yy2 + zz2)), (xy2 - zw2), (xz2 + yw2), translation[0]);
			mat[1] = newScale * make((xy2 + zw2), (1.0f - (xx2 + zz2)), (yz2 - xw2), translation[1]);
			mat[2] = newScale * make((xz2 - yw2), (yz2 + xw2), (1.0f - (xx2 + yy2)), translation[2]);
			mat[3] = make(0.0f, 0.0f, 0.0f, 1.0f);
		}

		static FORCEINLINE SSEVector make(uint32 x, uint32 y, uint32 z, uint32 w)
		{
			union { __m128 vecf; __m128i veci; } vecData;
			vecData.veci = _mm_setr_epi32(x, y, z, w);
			SSEVector result;
			result.data = vecData.vecf;
			return result;
		}

		static FORCEINLINE const SSEVector mask(uint32 index)
		{
			static const SSEVector masks[4] = {
				SSEVector::make((uint32)0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
				SSEVector::make((uint32)0xFFFFFFFF, 0, 0xFFFFFFFF, 0xFFFFFFFF),
				SSEVector::make((uint32)0xFFFFFFFF, 0xFFFFFFFF, 0, 0xFFFFFFFF),
				SSEVector::make((uint32)0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0) };
			LINA_CORE_ASSERT(index < 4, "Index is bigger than 4!");
			return masks[index];
		}

		static FORCEINLINE SSEVector make(float x, float y, float z, float w)
		{
			SSEVector vec;
			vec.data = _mm_setr_ps(x, y, z, w);
			return vec;
		}

		static FORCEINLINE SSEVector load4f(const float* vals)
		{
			SSEVector vec;
			vec.data = _mm_loadu_ps(vals);
			return vec;
		}

		static FORCEINLINE SSEVector load3f(const float* vals, float w)
		{
			return make(vals[0], vals[1], vals[2], w);
		}

		static FORCEINLINE SSEVector load1f(float val)
		{
			SSEVector vec;
			vec.data = _mm_set1_ps(val);
			return vec;
		}

		static FORCEINLINE SSEVector loadAligned(const float* vals)
		{
			SSEVector vec;
			vec.data = _mm_load_ps(vals);
			return vec;
		}

		static FORCEINLINE SSEVector set(float x, float y, float z)
		{
			return make(x, y, z, 0.0f);
		}

		static FORCEINLINE SSEVector set(float x, float y, float z, float w)
		{
			return make(x, y, z, w);
		}

		FORCEINLINE void store4f(float* result) const
		{
			_mm_storeu_ps(result, data);
		}

		FORCEINLINE void store3f(float* result) const
		{
			Memory::memcpy(result, &data, sizeof(float) * 3);
		}

		FORCEINLINE void store1f(float* result) const
		{
			_mm_store_ss(result, data);
		}

		FORCEINLINE void storeAligned(float* result) const
		{
			_mm_store_ps(result, data);
		}

		FORCEINLINE void storeAlignedStreamed(float* result) const
		{
			_mm_stream_ps(result, data);
		}

		FORCEINLINE SSEVector replicate(uint32 index) const
		{
			//assertCheck(index <= 3);
			return SSEVector::load1f((*this)[index]);
		}

		//	FORCEINLINE SSEVector swizzle(const uint32 x, const uint32 y, const uint32 z, const uint32 w) const
		//	{
		//		assertCheck(x <= 3);
		//		assertCheck(y <= 3);
		//		assertCheck(z <= 3);
		//		assertCheck(w <= 3);
		//		SSEVector vec;
		//		vec.data = _mm_shuffle_ps(data, data,
		//				SSEVector_SHUFFLEMASK(x, y, z, w));
		//		return vec;
		//	}

		FORCEINLINE SSEVector abs() const
		{
			static const SSEVector sign_mask(
				SSEVector::make((uint32)0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF));
			SSEVector vec;
			vec.data = _mm_and_ps(data, sign_mask.data);
			return vec;
		}

		FORCEINLINE SSEVector sign() const
		{
			static const SSEVector sign_mask(
				SSEVector::make((uint32)0x80000000, 0x80000000, 0x80000000, 0x80000000));
			SSEVector vec;
			vec.data = _mm_and_ps(data, sign_mask.data);
			return vec;
		}

		FORCEINLINE SSEVector min(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_min_ps(data, other.data);
			return vec;
		}

		FORCEINLINE SSEVector max(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_max_ps(data, other.data);
			return vec;
		}

		FORCEINLINE SSEVector neg() const
		{
			SSEVector vec;
			vec.data = _mm_xor_ps(data, _mm_set1_ps(-0.f));
			return vec;
		}

		FORCEINLINE SSEVector operator-() const
		{
			return neg();
		}

		FORCEINLINE SSEVector dot3(const SSEVector& other) const
		{
			const __m128 mul = _mm_mul_ps(data, other.data);
			const __m128 x = _mm_shuffle_ps(mul, mul, SSEVector_SHUFFLEMASK(0, 0, 0, 0));
			const __m128 y = _mm_shuffle_ps(mul, mul, SSEVector_SHUFFLEMASK(1, 1, 1, 1));
			const __m128 z = _mm_shuffle_ps(mul, mul, SSEVector_SHUFFLEMASK(2, 2, 2, 2));
			SSEVector vec;
			vec.data = _mm_add_ps(x, _mm_add_ps(y, z));
			return vec;
		}

		FORCEINLINE SSEVector dot4(const SSEVector& other) const
		{
			const __m128 t0 = _mm_mul_ps(data, other.data);
			SSEVector vec;
			vec.data = horizontalAdd(t0);
			return vec;
		}

		FORCEINLINE SSEVector cross3(const SSEVector& other) const
		{
			const __m128 t0 = _mm_shuffle_ps(data, data, SSEVector_SHUFFLEMASK(1, 2, 0, 3));
			const __m128 t1 = _mm_shuffle_ps(other.data, other.data,
				SSEVector_SHUFFLEMASK(1, 2, 0, 3));
			SSEVector vec;
			vec.data = _mm_sub_ps(_mm_mul_ps(data, t1), _mm_mul_ps(other.data, t0));
			vec.data = _mm_shuffle_ps(vec.data, vec.data,
				SSEVector_SHUFFLEMASK(1, 2, 0, 3));
			return vec;
		}

		FORCEINLINE SSEVector pow(const SSEVector& exp) const
		{
			return make(
				Math::Pow((*this)[0], exp[0]),
				Math::Pow((*this)[1], exp[1]),
				Math::Pow((*this)[2], exp[2]),
				Math::Pow((*this)[3], exp[3]));
		}

		FORCEINLINE SSEVector rsqrt() const
		{
			const SSEVector ONE(SSEVector::load1f(1.0f));
			SSEVector vec;
			vec.data = _mm_div_ps(ONE.data, _mm_sqrt_ps(data));
			return vec;
		}

		FORCEINLINE SSEVector reciprocal() const
		{
			const SSEVector ONE(SSEVector::load1f(1.0f));
			SSEVector vec;
			vec.data = _mm_div_ps(ONE.data, data);
			return vec;
		}

		FORCEINLINE SSEVector rlen4() const
		{
			return dot4(*this).rsqrt();
		}

		FORCEINLINE SSEVector rlen3() const
		{
			return dot3(*this).rsqrt();
		}

		FORCEINLINE SSEVector normalize4() const
		{
			return (*this) * rlen4();
		}

		FORCEINLINE SSEVector normalize3() const
		{
			return (*this) * rlen3();
		}

		FORCEINLINE void sincos(SSEVector* outSin, SSEVector* outCos) const
		{
			float outSinVals[4];
			float outCosVals[4];
			Math::SinCos(&outSinVals[0], &outCosVals[0], (*this)[0]);
			Math::SinCos(&outSinVals[1], &outCosVals[1], (*this)[1]);
			Math::SinCos(&outSinVals[2], &outCosVals[2], (*this)[2]);
			Math::SinCos(&outSinVals[3], &outCosVals[3], (*this)[3]);
			*outSin = SSEVector::load4f(outSinVals);
			*outCos = SSEVector::load4f(outCosVals);
		}

		FORCEINLINE SSEVector quatMul(const SSEVector& other) const
		{
			static const SSEVector mask(SSEVector::make(0.0f, 0.0f, 0.0f, -0.0f));
			SSEVector comp1, comp2, comp3;
			comp1.data = _mm_mul_ps(SSEVector_Swizzle(data, 0, 1, 2, 0), SSEVector_Swizzle(other.data, 3, 3, 3, 0));
			comp2.data = _mm_mul_ps(SSEVector_Swizzle(data, 1, 2, 0, 1), SSEVector_Swizzle(other.data, 2, 0, 1, 1));
			comp3.data = _mm_mul_ps(SSEVector_Swizzle(data, 2, 0, 1, 2), SSEVector_Swizzle(other.data, 1, 2, 0, 2));
			return replicate(3)*other - comp3 + ((comp1 + comp2) ^ mask);
		}

		FORCEINLINE SSEVector quatRotateVec(const SSEVector& vec) const
		{
			SSEVector tmp = SSEVector::load1f(2.0f) * cross3(vec);
			return vec + (tmp * replicate(3)) + cross3(tmp);
		}

		FORCEINLINE SSEVector mad(const SSEVector& mul, const SSEVector& add) const
		{
			SSEVector vec;
			vec.data = _mm_add_ps(_mm_mul_ps(data, mul.data), add.data);
			return vec;
		}

		FORCEINLINE SSEVector transform(const void* matrix) const
		{
			const SSEVector* m = (const SSEVector*)matrix;
			return make(dot4(m[0])[0], dot4(m[1])[0], dot4(m[2])[0], dot4(m[3])[0]);
		}

		FORCEINLINE SSEVector operator+(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_add_ps(data, other.data);
			return vec;
		}

		FORCEINLINE SSEVector operator-(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_sub_ps(data, other.data);
			return vec;
		}

		FORCEINLINE SSEVector operator*(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_mul_ps(data, other.data);
			return vec;
		}

		FORCEINLINE SSEVector operator/(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_div_ps(data, other.data);
			return vec;
		}

		FORCEINLINE bool isZero3f() const
		{
			return !(_mm_movemask_ps(data) & 0x07);
		}

		FORCEINLINE bool isZero4f() const
		{
			return !_mm_movemask_ps(data);
		}

		FORCEINLINE SSEVector operator==(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_cmpeq_ps(data, other.data);
			return vec;
		}

		FORCEINLINE SSEVector equals(const SSEVector& other, float errorMargin) const
		{
			return (*this - other).abs() < SSEVector::load1f(errorMargin);
		}

		FORCEINLINE SSEVector notEquals(const SSEVector& other, float errorMargin) const
		{
			return (*this - other).abs() >= SSEVector::load1f(errorMargin);
		}

		FORCEINLINE SSEVector operator!=(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_cmpneq_ps(data, other.data);
			return vec;
		}

		FORCEINLINE SSEVector operator>(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_cmpgt_ps(data, other.data);
			return vec;
		}

		FORCEINLINE SSEVector operator>=(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_cmpge_ps(data, other.data);
			return vec;
		}

		FORCEINLINE SSEVector operator<(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_cmplt_ps(data, other.data);
			return vec;
		}

		FORCEINLINE SSEVector operator<=(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_cmple_ps(data, other.data);
			return vec;
		}

		FORCEINLINE SSEVector operator|(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_or_ps(data, other.data);
			return vec;
		}

		FORCEINLINE SSEVector operator&(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_and_ps(data, other.data);
			return vec;
		}

		FORCEINLINE SSEVector operator^(const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_xor_ps(data, other.data);
			return vec;
		}

		FORCEINLINE float operator[](uint32 index) const
		{
			//assertCheck(index <= 3);
			return ((float*)&data)[index];
		}

		FORCEINLINE SSEVector select(const SSEVector& mask, const SSEVector& other) const
		{
			SSEVector vec;
			vec.data = _mm_xor_ps(other.data,
				_mm_and_ps(mask.data, _mm_xor_ps(data, other.data)));
			return vec;
		}

	private:
		__m128 data;

		static FORCEINLINE __m128 horizontalAdd(__m128 t0)
		{
#if SIMD_SUPPORTED_LEVEL >= SIMD_LEVEL_x86_SSSE3
			const __m128 t1 = _mm_hadd_ps(t0, t0);
			return _mm_hadd_ps(t1, t1);
#else
			const __m128 t1 = _mm_shuffle_ps(t0, t0, SSEVector_SHUFFLEMASK(2, 3, 0, 1));
			const __m128 t2 = _mm_add_ps(t1, t0);
			const __m128 t3 = _mm_shuffle_ps(t2, t2, SSEVector_SHUFFLEMASK(1, 2, 3, 0));
			return _mm_add_ps(t3, t2);
#endif
		}

		static FORCEINLINE __m128 mat2Mul(__m128 vec1, __m128 vec2)
		{
			return
				_mm_add_ps(_mm_mul_ps(vec1, SSEVector_Swizzle(vec2, 0, 3, 0, 3)),
					_mm_mul_ps(SSEVector_Swizzle(vec1, 1, 0, 3, 2), SSEVector_Swizzle(vec2, 2, 1, 2, 1)));
		}
		// 2x2 row major Matrix adjugate multiply (A#)*B
		static FORCEINLINE __m128 mat2AdjMul(__m128 vec1, __m128 vec2)
		{
			return
				_mm_sub_ps(_mm_mul_ps(SSEVector_Swizzle(vec1, 3, 3, 0, 0), vec2),
					_mm_mul_ps(SSEVector_Swizzle(vec1, 1, 1, 2, 2), SSEVector_Swizzle(vec2, 2, 3, 0, 1)));

		}
		// 2x2 row major Matrix multiply adjugate A*(B#)
		static FORCEINLINE __m128 mat2MulAdj(__m128 vec1, __m128 vec2)
		{
			return
				_mm_sub_ps(_mm_mul_ps(vec1, SSEVector_Swizzle(vec2, 3, 0, 3, 0)),
					_mm_mul_ps(SSEVector_Swizzle(vec1, 1, 0, 3, 2), SSEVector_Swizzle(vec2, 2, 1, 2, 1)));
		}
	};
}


#endif