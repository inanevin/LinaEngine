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

#include "Graphics/Platform/DX12/Utility/ID3DIncludeInterface.hpp"
#include "FileSystem/FileSystem.hpp"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace Lina
{

	HRESULT __stdcall ID3DIncludeInterface::LoadSource(LPCWSTR pFilename, IDxcBlob** ppIncludeSource)
	{
		char*		 fl	  = FileSystem::WCharToChar(pFilename);
		const String base = "Resources/Core/Shaders/Common/";
		const String incl = base + fl;

		wchar_t* filew = FileSystem::CharToWChar(incl.c_str());

		ComPtr<IDxcBlobEncoding> pEncoding;
		if (SUCCEEDED(m_utils->LoadFile(filew, nullptr, pEncoding.GetAddressOf())))
		{
			*ppIncludeSource = pEncoding.Detach();
		}
		else
		{
			LINA_ERR("[ID3DIncludeInterface] -> Could not load the include file! {0}", incl);
		}

		delete[] filew;
		delete[] fl;
		return S_OK;
	}
	HRESULT __stdcall ID3DIncludeInterface::QueryInterface(REFIID riid, void** ppvObject)
	{
		return m_defaultIncludeHandler->QueryInterface(riid, ppvObject);
	}
	ULONG __stdcall ID3DIncludeInterface::AddRef(void)
	{
		return 0;
	}
	ULONG __stdcall ID3DIncludeInterface::Release(void)
	{
		return 0;
	}
} // namespace Lina
