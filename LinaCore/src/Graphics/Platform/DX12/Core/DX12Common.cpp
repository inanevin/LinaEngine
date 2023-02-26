/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted: free of charge: to any person obtaining a copy
of this software and associated documentation files (the "Software"): to deal
in the Software without restriction: including without limitation the rights
to use: copy: modify: merge: publish: distribute: sublicense: and/or sell
copies of the Software: and to permit persons to whom the Software is
furnished to do so: subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS": WITHOUT WARRANTY OF ANY KIND: EXPRESS OR
IMPLIED: INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY:
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM: DAMAGES OR OTHER
LIABILITY: WHETHER IN AN ACTION OF CONTRACT: TORT OR OTHERWISE: ARISING FROM:
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Graphics/Platform/DX12/Core/DX12Common.hpp"

namespace Lina
{
	DXGI_FORMAT GetFormat(Format format)
	{
		switch (format)
		{
		case Format::UNDEFINED:
			return DXGI_FORMAT_UNKNOWN;
		case Format::B8G8R8A8_SRGB:
			return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		case Format::B8G8R8A8_UNORM:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		case Format::R32G32B32_SFLOAT:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case Format::R32G32B32A32_SFLOAT:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case Format::R32G32_SFLOAT:
			return DXGI_FORMAT_R32G32_FLOAT;
		case Format::D32_SFLOAT:
			return DXGI_FORMAT_R32_FLOAT;
		case Format::R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Format::R8G8B8A8_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case Format::R8G8_UNORM:
			return DXGI_FORMAT_R8G8_UNORM;
		case Format::R8_UNORM:
			return DXGI_FORMAT_R8_UNORM;
		case Format::R8_UINT:
			return DXGI_FORMAT_R8_UINT;
		case Format::R16_SFLOAT:
			return DXGI_FORMAT_R16_FLOAT;
		case Format::R16_SINT:
			return DXGI_FORMAT_R16_SINT;
		case Format::R32_SFLOAT:
			return DXGI_FORMAT_R32_FLOAT;
		case Format::R32_SINT:
			return DXGI_FORMAT_R32_SINT;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}

		return DXGI_FORMAT_UNKNOWN;
	}
	D3D12_COMMAND_LIST_TYPE GetCommandType(CommandType type)
	{
		switch (type)
		{
		case CommandType::Graphics:
			return D3D12_COMMAND_LIST_TYPE_DIRECT;
		case CommandType::Compute:
			return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		case CommandType::Transfer:
			return D3D12_COMMAND_LIST_TYPE_COPY;
		default:
			return D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
	}
} // namespace Lina
