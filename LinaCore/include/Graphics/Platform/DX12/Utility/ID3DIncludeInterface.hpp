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

#ifndef ID3DIncludeInterface_HPP
#define ID3DIncludeInterface_HPP

#include "Graphics/Platform/DX12/SDK/d3dcommon.h"
#include <dxcapi/dxcapi.h>
#include <wrl/client.h>

namespace Lina
{
	class ID3DIncludeInterface : public IDxcIncludeHandler
	{
	public:
		ID3DIncludeInterface()
		{
		}
		virtual ~ID3DIncludeInterface()
		{
		}

		virtual HRESULT STDMETHODCALLTYPE LoadSource(_In_z_ LPCWSTR							  pFilename,	  // Candidate filename.
													 _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource // Resultant source object for included file, nullptr if not found.
													 ) override;

	protected:
		// Inherited via IDxcIncludeHandler
		virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override;
		virtual ULONG __stdcall AddRef(void) override;
		virtual ULONG __stdcall Release(void) override;

	private:
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> m_defaultIncludeHandler;
	};

} // namespace Lina

#endif
