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

#include "UndoAction.hpp"
#include "Core/Resources/CommonResources.hpp"
#include "Common/Data/String.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Texture.hpp"

namespace Lina
{
	struct ResourceDirectory;
};

namespace Lina::Editor
{
	class Editor;

	class UndoActionResourceRename : public UndoAction
	{
	public:
		UndoActionResourceRename()			= default;
		virtual ~UndoActionResourceRename() = default;

		static UndoActionResourceRename* Create(Editor* editor, ResourceDirectory* dir, const String& newName);
		virtual void					 Execute(Editor* editor) override;
		virtual void					 Undo(Editor* editor) override;

	private:
		String m_prevName	  = "";
		String m_newName	  = "";
		uint64 m_resourceGUID = 0;
	};

	class UndoActionSamplerDataChanged : public UndoAction
	{
	public:
		UndoActionSamplerDataChanged()			= default;
		virtual ~UndoActionSamplerDataChanged() = default;

		static UndoActionSamplerDataChanged* Create(Editor* editor, ResourceID resourceID, const LinaGX::SamplerDesc& prevDesc);
		virtual void						 Execute(Editor* editor) override;
		virtual void						 Undo(Editor* editor) override;

	private:
		ResourceID			m_resourceID = 0;
		LinaGX::SamplerDesc m_prevDesc	 = {};
	};

	class UndoActionFontDataChanged : public UndoAction
	{
	public:
		UndoActionFontDataChanged()			 = default;
		virtual ~UndoActionFontDataChanged() = default;

		static UndoActionFontDataChanged* Create(Editor* editor, ResourceID resourceID, const Font::Metadata& meta);
		virtual void					  Execute(Editor* editor) override;
		virtual void					  Undo(Editor* editor) override;

	private:
		ResourceID	   m_resourceID = 0;
		Font::Metadata m_prevMeta	= {};
	};

	class UndoActionTextureDataChanged : public UndoAction
	{
	public:
		UndoActionTextureDataChanged()			= default;
		virtual ~UndoActionTextureDataChanged() = default;

		static UndoActionTextureDataChanged* Create(Editor* editor, ResourceID resourceID, const Texture::Metadata& meta);
		virtual void						 Execute(Editor* editor) override;
		virtual void						 Undo(Editor* editor) override;

	private:
		void Apply(Editor* editor, bool applyMeta);

	private:
		ResourceID		  m_resourceID = 0;
		Texture::Metadata m_prevMeta   = {};
	};

	class UndoActionMaterialDataChanged : public UndoAction
	{
	public:
		UndoActionMaterialDataChanged() = default;
		virtual ~UndoActionMaterialDataChanged()
		{
			m_prevStream.Destroy();
		};

		static UndoActionMaterialDataChanged* Create(Editor* editor, ResourceID resourceID, const OStream& prevStream, uint64 resourceSpace);
		virtual void						  Execute(Editor* editor) override;
		virtual void						  Undo(Editor* editor) override;

	private:
		void Apply(Editor* editor, bool applyMeta);

	private:
		ResourceID m_resourceID	   = 0;
		uint64	   m_resourceSpace = 0;
		OStream	   m_prevStream	   = {};
	};

	class UndoActionMaterialShaderChanged : public UndoAction
	{
	public:
		UndoActionMaterialShaderChanged()		   = default;
		virtual ~UndoActionMaterialShaderChanged() = default;

		static UndoActionMaterialShaderChanged* Create(Editor* editor, ResourceID resourceID, ResourceID prevShader, ResourceID newShader);
		virtual void							Execute(Editor* editor) override;
		virtual void							Undo(Editor* editor) override;

	private:
		void Apply(Editor* editor, bool applyMeta);

	private:
		ResourceID m_resourceID = 0;
		ResourceID m_prevShader = 0;
		ResourceID m_newShader	= 0;
		OStream	   m_prevStream = {};
	};
} // namespace Lina::Editor
