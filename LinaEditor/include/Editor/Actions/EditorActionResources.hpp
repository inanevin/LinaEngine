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

#include "EditorAction.hpp"
#include "Common/Data/String.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include <LinaGX/Common/Defines/SamplerDesc.hpp>

namespace Lina
{
	struct ResourceDirectory;
};

namespace Lina::Editor
{
	class Editor;

	class EditorActionResourceRename : public EditorAction
	{
	public:
		EditorActionResourceRename()		  = default;
		virtual ~EditorActionResourceRename() = default;

		static EditorActionResourceRename* Create(Editor* editor, GUID GUID, const String& oldName, const String& newName);
		virtual void					   Execute(Editor* editor, ExecType type) override;

	private:
		String m_prevName	  = "";
		String m_newName	  = "";
		GUID   m_resourceGUID = 0;
	};

	class EditorActionResourceMove : public EditorAction
	{
	public:
		EditorActionResourceMove()			= default;
		virtual ~EditorActionResourceMove() = default;

		static EditorActionResourceMove* Create(Editor* editor, const Vector<GUID>& resources, const Vector<GUID>& previousParents, GUID newParent);
		virtual void					 Execute(Editor* editor, ExecType type) override;

	private:
		Vector<GUID> m_resourceGUIDs;
		Vector<GUID> m_previousParentGUIDs;
		GUID		 m_newParentGUIDs = 0;
	};

	class EditorActionResourceFav : public EditorAction
	{
	public:
		EditorActionResourceFav()		   = default;
		virtual ~EditorActionResourceFav() = default;

		static EditorActionResourceFav* Create(Editor* editor, const Vector<GUID>& resources, bool isAdd);
		virtual void					Execute(Editor* editor, ExecType type) override;

	private:
		Vector<GUID> m_resourceGUIDs;
		bool		 m_isAdd = false;
	};

	class EditorActionResourceSampler : public EditorAction
	{
	public:
		EditorActionResourceSampler()		   = default;
		virtual ~EditorActionResourceSampler() = default;

		static EditorActionResourceSampler* Create(Editor* editor, ResourceID resourceID, const LinaGX::SamplerDesc& prevDesc, const LinaGX::SamplerDesc& newDesc);
		virtual void						Execute(Editor* editor, ExecType type) override;

	private:
		ResourceID			m_resourceID = 0;
		LinaGX::SamplerDesc m_prevDesc	 = {};
		LinaGX::SamplerDesc m_newDesc	 = {};
	};

	class EditorActionResourceFont : public EditorAction
	{
	public:
		EditorActionResourceFont()			= default;
		virtual ~EditorActionResourceFont() = default;

		static EditorActionResourceFont* Create(Editor* editor, ResourceID resourceID, const Font::Metadata& prevMeta, const Font::Metadata& newMeta);
		virtual void					 Execute(Editor* editor, ExecType type) override;

	private:
		ResourceID	   m_resourceID = 0;
		Font::Metadata m_prevMeta	= {};
		Font::Metadata m_newMeta	= {};
	};

	class EditorActionResourceTexture : public EditorAction
	{
	public:
		EditorActionResourceTexture()		   = default;
		virtual ~EditorActionResourceTexture() = default;

		static EditorActionResourceTexture* Create(Editor* editor, ResourceID resourceID, const Texture::Metadata& prev, const Texture::Metadata& newMeta);
		virtual void						Execute(Editor* editor, ExecType type) override;

	private:
		ResourceID		  m_resourceID = 0;
		Texture::Metadata m_prevMeta   = {};
		Texture::Metadata m_newMeta	   = {};
	};

	class EditorActionResourceMaterial : public EditorAction
	{
	public:
		EditorActionResourceMaterial() = default;
		virtual ~EditorActionResourceMaterial()
		{
			m_prevStream.Destroy();
			m_newStream.Destroy();
		};

		static EditorActionResourceMaterial* Create(Editor* editor, ResourceID resourceID, uint64 resourceSpace, const OStream& prevStream, const OStream& newStream);
		virtual void						 Execute(Editor* editor, ExecType type) override;

	private:
		ResourceID m_resourceID	   = 0;
		uint64	   m_resourceSpace = 0;
		OStream	   m_prevStream	   = {};
		OStream	   m_newStream	   = {};
	};

	class EditorActionResourceModel : public EditorAction
	{
	public:
		EditorActionResourceModel() = default;
		virtual ~EditorActionResourceModel()
		{
			m_prevStream.Destroy();
			m_newStream.Destroy();
		};

		static EditorActionResourceModel* Create(Editor* editor, ResourceID resourceID, uint64 resourceSpace, const OStream& prevStream, const OStream& newStream);
		virtual void					  Execute(Editor* editor, ExecType type) override;

	private:
		ResourceID m_resourceID	   = 0;
		uint64	   m_resourceSpace = 0;
		OStream	   m_prevStream	   = {};
		OStream	   m_newStream	   = {};
	};

	class EditorActionResourceMaterialShader : public EditorAction
	{
	public:
		EditorActionResourceMaterialShader() = default;
		virtual ~EditorActionResourceMaterialShader()
		{
			m_prevStream.Destroy();
		}

		static EditorActionResourceMaterialShader* Create(Editor* editor, ResourceID resourceID, uint64 resourceSpace, ResourceID prevShader, ResourceID newShader, const OStream& prevStream);
		virtual void							   Execute(Editor* editor, ExecType type) override;

	private:
		ResourceID m_resourceID	   = 0;
		ResourceID m_prevShader	   = 0;
		ResourceID m_newShader	   = 0;
		uint64	   m_resourceSpace = 0;
		OStream	   m_prevStream	   = {};
	};
} // namespace Lina::Editor
