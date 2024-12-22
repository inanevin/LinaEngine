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

#include "Common/Data/Vector.hpp"
#include "Common/Math/Matrix.hpp"
#include "Mesh.hpp"

namespace Lina
{
	enum class ModelAnimationInterpolation
	{
		Linear,
		Step,
		CubicSpline,
	};

	struct ModelAnimationKeyframeV3
	{
		float	time  = 0.0f;
		Vector3 value = Vector3::Zero;

		void SaveToStream(OStream& stream) const
		{
			stream << time << value;
		}
		void LoadFromStream(IStream& stream)
		{
			stream >> time >> value;
		}
	};

	struct ModelAnimationKeyframeV3Spline
	{
		float	time	   = 0.0f;
		Vector3 inTangent  = Vector3::Zero;
		Vector3 value	   = Vector3::Zero;
		Vector3 outTangent = Vector3::Zero;

		void SaveToStream(OStream& stream) const
		{
			stream << time << value << inTangent << outTangent;
		}
		void LoadFromStream(IStream& stream)
		{
			stream >> time >> value >> inTangent >> outTangent;
		}
	};

	struct ModelAnimationKeyframeQ
	{
		float	   time	 = 0.0f;
		Quaternion value = Quaternion();

		void SaveToStream(OStream& stream) const
		{
			stream << time << value;
		}
		void LoadFromStream(IStream& stream)
		{
			stream >> time >> value;
		}
	};

	struct ModelAnimationKeyframeQSpline
	{
		float	   time		  = 0.0f;
		Quaternion inTangent  = Quaternion();
		Quaternion value	  = Quaternion();
		Quaternion outTangent = Quaternion();

		void SaveToStream(OStream& stream) const
		{
			stream << time << value << inTangent << outTangent;
		}
		void LoadFromStream(IStream& stream)
		{
			stream >> time >> value >> inTangent >> outTangent;
		}
	};

	struct ModelAnimationChannelV3
	{
		ModelAnimationInterpolation			   interpolation = ModelAnimationInterpolation::Linear;
		uint32								   nodeIndex	 = 0;
		Vector<ModelAnimationKeyframeV3>	   keyframes;
		Vector<ModelAnimationKeyframeV3Spline> keyframesSpline;

		Vector3 Sample(float time) const;

		void SaveToStream(OStream& stream) const
		{
			stream << interpolation << nodeIndex << keyframes << keyframesSpline;
		}

		void LoadFromStream(IStream& stream)
		{
			stream >> interpolation >> nodeIndex >> keyframes >> keyframesSpline;
		}
	};

	struct ModelAnimationChannelQ
	{
		ModelAnimationInterpolation			  interpolation = ModelAnimationInterpolation::Linear;
		uint32								  nodeIndex		= 0;
		Vector<ModelAnimationKeyframeQ>		  keyframes;
		Vector<ModelAnimationKeyframeQSpline> keyframesSpline;

		Quaternion Sample(float time) const;

		void SaveToStream(OStream& stream) const
		{
			stream << interpolation << nodeIndex << keyframes << keyframesSpline;
		}

		void LoadFromStream(IStream& stream)
		{
			stream >> interpolation >> nodeIndex >> keyframes >> keyframesSpline;
		}
	};

	struct ModelAnimation
	{
		String							name	 = "";
		float							duration = 0.0f;
		Vector<ModelAnimationChannelV3> positionChannels;
		Vector<ModelAnimationChannelV3> scaleChannels;
		Vector<ModelAnimationChannelQ>	rotationChannels;

		void SaveToStream(OStream& stream) const
		{
			stream << name << duration << positionChannels << scaleChannels << rotationChannels;
		}

		void LoadFromStream(IStream& stream)
		{
			stream >> name >> duration >> positionChannels >> scaleChannels >> rotationChannels;
		}
	};
} // namespace Lina
