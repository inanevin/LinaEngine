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

#include "Editor/World/WorldUtility.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Editor/CommonEditor.hpp"
#include "Core/Resources/ResourceManager.hpp"

namespace Lina::Editor
{
	void WorldUtility::SetupDefaultSkyMaterial(Material* mat, ResourceManagerV2* rm)
	{
		if (rm)
			mat->SetShader(rm->GetResource<Shader>(EDITOR_SHADER_DEFAULT_SKY_ID));
		mat->SetProperty("skyColor"_hs, Vector3(0.38f, 0.521f, 0.807f));
		mat->SetProperty("horizonColor"_hs, Vector3(0.386f, 0.521f, 0.807f));
		mat->SetProperty("groundColor"_hs, Vector3(0.011f, 0.021f, 0.05f));
		mat->SetProperty("sunPosition"_hs, Vector2(0.0f, 0.0f));
		mat->SetProperty("sunColor"_hs, Vector3(1.0f, 1.0f, 1.0f));
		mat->SetProperty("sunSize"_hs, 0.0f);
		mat->SetProperty("sunDiffusion"_hs, 5.0f);
		mat->SetProperty("horizonPosition"_hs, 0.0f);
		mat->SetProperty("horizonDiffusion"_hs, 0.15f);
	}
} // namespace Lina::Editor
