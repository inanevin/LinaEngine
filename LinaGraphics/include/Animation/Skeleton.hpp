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

/*
Class: Skeleton



Timestamp: 12/7/2021 2:13:56 PM
*/

#pragma once

#ifndef Skeleton_HPP
#define Skeleton_HPP

// Headers here.

#include <ozz/animation/runtime/skeleton.h>
#include <ozz/base/maths/soa_transform.h>
#include <ozz/base/containers/vector.h>
#include <ozz/base/maths/soa_float4x4.h>
#include <ozz/animation/runtime/sampling_job.h>
#include <string>
#include <map>

namespace LinaEngine::Graphics
{
	class Animation;

	class Skeleton
	{
		
	public:
		
		Skeleton() {};
		~Skeleton();
	
		bool LoadSkeleton(const std::string& path);

		bool IsLoaded() const { return m_loaded; }

		std::map<std::string, Animation*>& GetAnimations()
		{
			return m_animationMap;
		}

		ozz::animation::Skeleton& GetSkeleton() { return m_skeleton; }
		ozz::vector<ozz::math::SoaTransform>& GetLocals() { return m_locals; }
		ozz::vector<ozz::math::Float4x4>& GetModels() { return m_models; }
		ozz::animation::SamplingCache& GetCache() { return m_cache; }

	private:


		bool m_loaded = false;
		ozz::animation::Skeleton m_skeleton;
		ozz::vector<ozz::math::SoaTransform> m_locals;
		ozz::vector<ozz::math::Float4x4> m_models;
		ozz::animation::SamplingCache m_cache;
		std::map<std::string, Animation*> m_animationMap;

	};
}

#endif
