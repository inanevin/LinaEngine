/*
Class: ShaderInclude



Timestamp: 12/31/2021 5:09:30 PM
*/

#pragma once

#ifndef ShaderInclude_HPP
#define ShaderInclude_HPP

// Headers here.
#include "Resources/IResource.hpp"

namespace Lina::Graphics
{
	class ShaderInclude : public Resources::IResource
	{
		
	public:
		
		virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const char* path) override;
	
		const String& GetText() const
        {
			return m_text;
		}
	private:
	
		String m_text;

	};
}

#endif
