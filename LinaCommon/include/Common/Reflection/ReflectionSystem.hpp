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

#include "Common/StringID.hpp"
#include "Common/Data/Functional.hpp"
#include <type_traits>

namespace Lina
{
	class PropertyCacheBase
	{
	public:
		PropertyCacheBase()			 = default;
		virtual ~PropertyCacheBase() = default;
	};

	template <typename T> class PropertyCache : public PropertyCacheBase
	{
	private:
		struct PropertyPair
		{
			StringID id	 = 0;
			T		 val = T();
		};

	public:
		void AddProperty(StringID sid, T p)
		{
			if (HasProperty(sid))
				return;

			m_properties.push_back({sid, p});
			_order.push_back(sid);
		}

		T GetProperty(StringID sid)
		{
			PropertyPair out = {};
			if (!FindPair(sid, out))
			{
				LINA_ASSERT(false, "");
			}
			return out.val;
		}

		bool HasProperty(StringID sid) const
		{
			PropertyPair out = {};
			return FindPair(sid, out);
		}

		Vector<T> GetSortedVector()
		{
			Vector<T>	 vec;
			PropertyPair out = {};
			for (StringID sid : _order)
			{
				if (FindPair(sid, out))
					vec.push_back(out.val);
			}
			return vec;
		}

	private:
		bool FindPair(StringID id, PropertyPair& pair) const
		{
			for (const PropertyPair& p : m_properties)
			{
				if (p.id == id)
				{
					pair = p;
					return true;
				}
			}

			return false;
		}

	private:
		Vector<PropertyPair> m_properties;
		Vector<StringID>	 _order;
	};

	struct PropertyCachePair
	{
		TypeID			   tid	 = 0;
		PropertyCacheBase* cache = nullptr;
	};

	class PropertyCacheManager
	{
	private:
	public:
		PropertyCacheManager() = default;

		virtual ~PropertyCacheManager()
		{
		}

		template <typename T> bool HasProperty(StringID sid) const
		{
			const TypeID	  tid = GetTypeID<T>();
			PropertyCachePair out = {};
			if (!FindPair(tid, out))
				return false;
			return static_cast<PropertyCache<T>*>(out.cache)->HasProperty(sid);
		}

		template <typename T> inline void AddProperty(StringID sid, T param)
		{
			const TypeID tid = GetTypeID<T>();

			PropertyCachePair out = {};

			if (!FindPair(tid, out))
			{
				PropertyCache<T>* cache = new PropertyCache<T>();
				m_propertyCaches.push_back({.tid = tid, .cache = cache});
				cache->AddProperty(sid, param);
			}
			else
			{
				PropertyCache<T>* cache = static_cast<PropertyCache<T>*>(out.cache);
				cache->AddProperty(sid, param);
			}
		}

		template <typename T> inline PropertyCache<T>* GetPropertyCache() const
		{
			const TypeID	  tid = GetTypeID<T>();
			PropertyCachePair out = {};

			if (!FindPair(tid, out))
			{
				LINA_ASSERT(false, "");
				return nullptr;
			}

			return static_cast<PropertyCache<T>*>(out.cache);
		}

		template <typename T> inline T GetProperty(StringID sid) const
		{
			return GetPropertyCache<T>()->GetProperty(sid);
		}

		void Destroy()
		{
			for (PropertyCachePair& pair : m_propertyCaches)
				delete pair.cache;
			m_propertyCaches.clear();
		}

		const Vector<PropertyCachePair>& GetPropertyCaches() const
		{
			return m_propertyCaches;
		}

	private:
		bool FindPair(TypeID tid, PropertyCachePair& pair) const
		{
			for (const PropertyCachePair& p : m_propertyCaches)
			{
				if (p.tid == tid)
				{
					pair = p;
					return true;
				}
			}

			return false;
		}

	private:
		Vector<PropertyCachePair> m_propertyCaches;
	};

	class FieldValue
	{
	public:
		FieldValue(){};
		FieldValue(void* addr) : ptr(addr){};
		template <typename T> T GetValue()
		{
			return Cast<T>();
		}

		template <typename T> void SetValue(T t)
		{
			Cast<T>() = t;
		}

		template <typename T> T Cast()
		{
			return *static_cast<T*>(ptr);
		}

		template <typename T> T& CastRef()
		{
			return *static_cast<T*>(ptr);
		}

		template <typename T> T* CastPtr()
		{
			return static_cast<T*>(ptr);
		}

		void* GetPtr() const
		{
			return ptr;
		}

	private:
		template <typename T, typename U> friend class Field;
		void* ptr = nullptr;
	};

	class FunctionCacheBase
	{
	public:
		FunctionCacheBase()			 = default;
		virtual ~FunctionCacheBase() = default;
	};

	template <typename T> class FunctionCache : public FunctionCacheBase
	{
	private:
		struct FunctionPair
		{
			StringID	id	 = 0;
			Delegate<T> func = {};
		};

	public:
		void AddFunction(StringID sid, Delegate<T>&& f)
		{
			FunctionPair out = {};
			if (FindPair(sid, out))
			{
				out.func = f;
				return;
			}

			m_functions.push_back({.id = sid, .func = f});
		}

		Delegate<T> GetFunction(StringID sid)
		{
			FunctionPair out = {};
			if (!FindPair(sid, out))
			{
				LINA_ASSERT(false, "");
			}
			return out.func;
		}

		bool HasFunction(StringID sid)
		{
			FunctionPair out = {};
			return FindPair(sid, out);
		}

		FunctionCache()			 = default;
		virtual ~FunctionCache() = default;

	private:
		bool FindPair(TypeID id, FunctionPair& pair) const
		{
			for (const FunctionPair& f : m_functions)
			{
				if (f.id == id)
				{
					pair = f;
					return true;
				}
			}

			return false;
		}

	private:
		Vector<FunctionPair> m_functions;
	};

	struct FunctionCachePair
	{
		TypeID			   tid	 = 0;
		FunctionCacheBase* cache = nullptr;
	};

	struct DependencyPair
	{
		StringID	  id = 0;
		Vector<uint8> deps;
		StringID	  operation = 0;
	};

	class FieldBase
	{

	private:
	public:
		FieldBase() = default;
		virtual ~FieldBase()
		{
			for (FunctionCachePair& pair : m_functionCaches)
				delete pair.cache;

			m_functionCaches.clear();
			m_propertyCacheManager.Destroy();
		}

		virtual void	   SaveToStream(OStream& stream, void* obj)	  = 0;
		virtual void	   LoadFromStream(IStream& stream, void* obj) = 0;
		virtual FieldValue Value(void* obj)							  = 0;
		virtual size_t	   GetTypeSize() const						  = 0;

		template <typename T> inline void AddProperty(StringID sid, T param)
		{
			m_propertyCacheManager.AddProperty<T>(sid, param);
		}

		template <typename T> inline T GetProperty(StringID sid)
		{
			return m_propertyCacheManager.GetProperty<T>(sid);
		}

		template <typename T> inline bool HasProperty(StringID sid)
		{
			return m_propertyCacheManager.HasProperty<T>(sid);
		}

		const Vector<PropertyCachePair>& GetPropertyCaches() const
		{
			return m_propertyCacheManager.GetPropertyCaches();
		}

		template <typename T> inline void AddFunction(StringID sid, Delegate<T>&& del)
		{
			const TypeID tid = GetTypeID<T>();

			FunctionCachePair out = {};

			if (!FindFunctionCache(tid, out))
			{
				FunctionCache<T>* cache = new FunctionCache<T>();
				m_functionCaches.push_back({tid, cache});
				cache->AddFunction(sid, linatl::move(del));
			}
			else
			{
				FunctionCache<T>* cache = static_cast<FunctionCache<T>*>(out.cache);

				if (cache->HasFunction(sid))
					return;
				cache->AddFunction(sid, linatl::move(del));
			}
		}

		template <typename T> inline Delegate<T> GetFunction(StringID sid)
		{
			const TypeID tid = GetTypeID<T>();

			FunctionCachePair out = {};

			if (!FindFunctionCache(tid, out))
			{
				LINA_ASSERT(false, "");
			}

			FunctionCache<T>* cache = static_cast<FunctionCache<T>*>(out.cache);
			return cache->GetFunction(sid);
		}

		inline void AddDependency(StringID sid, const String& valuesRaw, StringID operation = "eq"_hs)
		{
			const String		 trimmed   = UtilStr::RemoveWhitespaces(valuesRaw);
			const Vector<String> valuesStr = UtilStr::SplitBy(trimmed, ",");

			Vector<uint8> values;

			for (const String& str : valuesStr)
			{
				values.push_back(static_cast<uint8>(UtilStr::StringToInt(str)));
			}

			DependencyPair out = {};
			if (FindPosDepCache(sid, out))
			{
				out.deps = values;
				return;
			}

			m_positiveDependencies.push_back({sid, values, operation});
		}

		const Vector<DependencyPair>& GetPositiveDependencies() const
		{
			return m_positiveDependencies;
		}

		int32 _order = 0;

		inline bool* GetFoldValuePtr()
		{
			return &m_foldValue;
		}

	private:
		bool FindFunctionCache(TypeID tid, FunctionCachePair& pair) const
		{
			for (const FunctionCachePair& f : m_functionCaches)
			{
				if (f.tid == tid)
				{
					pair = f;
					return true;
				}
			}

			return false;
		}

		bool FindPosDepCache(StringID id, DependencyPair& pair) const
		{
			for (const DependencyPair& d : m_positiveDependencies)
			{
				if (d.id == id)
				{
					pair = d;
					return true;
				}
			}

			return false;
		}

	private:
		bool					  m_foldValue = false;
		Vector<DependencyPair>	  m_positiveDependencies;
		Vector<FunctionCachePair> m_functionCaches;
		PropertyCacheManager	  m_propertyCacheManager;
	};

	template <typename T, class C> class Field : public FieldBase
	{
	public:
		Field()			 = default;
		virtual ~Field() = default;

		virtual size_t GetTypeSize() const override
		{
			return sizeof(T);
		}

		inline virtual void SaveToStream(OStream& stream, void* obj) override
		{
			FieldValue value = Value(obj);
			stream << value.GetValue<T>();
		}

		inline virtual void LoadFromStream(IStream& stream, void* obj) override
		{
			FieldValue value = Value(obj);
			// stream >> *(value.GetPtr());
		}

		inline virtual FieldValue Value(void* obj) override
		{
			FieldValue val;
			val.ptr = &((static_cast<C*>(obj))->*(m_var));
			return val;
		}

		T m_var = T();
	};

	struct FieldPair
	{
		StringID   sid	 = 0;
		FieldBase* field = nullptr;
	};

	class MetaType
	{
	public:
		inline FieldBase* GetField(StringID sid)
		{
			FieldPair out = {};
			if (!FindField(sid, out))
			{
				LINA_ASSERT(false, "");
			}
			return out.field;
		}

		template <auto DATA, typename Class> void AddField(StringID sid)
		{
			Field<decltype(DATA), Class>* f = new Field<decltype(DATA), Class>();
			f->m_var						= DATA;
			f->_order						= static_cast<int32>(m_fields.size());
			m_fields.push_back({sid, f});
		}

		inline bool HasField(StringID sid) const
		{
			FieldPair out = {};
			return FindField(sid, out);
		}

		template <typename T> inline void AddProperty(StringID sid, T param)
		{
			m_propertyCacheManager.AddProperty<T>(sid, param);
		}

		template <typename T> inline T GetProperty(StringID sid) const
		{
			return m_propertyCacheManager.GetProperty<T>(sid);
		}

		template <typename T> bool HasProperty(StringID sid) const
		{
			return m_propertyCacheManager.HasProperty<T>(sid);
		}

		template <typename T> inline void AddFunction(StringID sid, Delegate<T>&& del)
		{
			const TypeID tid = GetTypeID<T>();

			FunctionCachePair out = {};

			if (!FindFunctionCache(tid, out))
			{
				FunctionCache<T>* cache = new FunctionCache<T>();
				m_functionCaches.push_back({tid, cache});
				cache->AddFunction(sid, linatl::move(del));
			}
			else
			{
				FunctionCache<T>* cache = static_cast<FunctionCache<T>*>(out.cache);

				if (cache->HasFunction(sid))
					return;
				cache->AddFunction(sid, linatl::move(del));
			}
		}

		template <typename T> inline Delegate<T> GetFunction(StringID sid)
		{
			const TypeID tid = GetTypeID<T>();

			FunctionCachePair out = {};

			if (!FindFunctionCache(tid, out))
			{
				LINA_ASSERT(false, "");
			}

			FunctionCache<T>* cache = static_cast<FunctionCache<T>*>(out.cache);
			return cache->GetFunction(sid);
		}

		const Vector<FieldPair>& GetFields() const
		{
			return m_fields;
		}

		Vector<FieldBase*> GetFieldsOrdered() const
		{
			Vector<FieldBase*> fields;
			fields.reserve(m_fields.size());

			for (const FieldPair& pair : m_fields)
				fields.push_back(pair.field);

			linatl::sort(fields.begin(), fields.end(), [](FieldBase* f1, FieldBase* f2) -> bool { return f1->_order < f2->_order; });
			return fields;
		}

		inline PropertyCacheManager& GetPropertyCacheManager()
		{
			return m_propertyCacheManager;
		}

		inline bool* GetFoldValuePtr()
		{
			return &m_foldValue;
		}

	private:
		bool FindFunctionCache(TypeID tid, FunctionCachePair& pair) const
		{
			for (const FunctionCachePair& f : m_functionCaches)
			{
				if (f.tid == tid)
				{
					pair = f;
					return true;
				}
			}

			return false;
		}

		bool FindField(StringID sid, FieldPair& pair) const
		{
			for (const FieldPair& f : m_fields)
			{
				if (f.sid == sid)
				{
					pair = f;
					return true;
				}
			}

			return false;
		}

	private:
		friend class ReflectionSystem;
		bool					  m_foldValue = false;
		PropertyCacheManager	  m_propertyCacheManager;
		Vector<FunctionCachePair> m_functionCaches;
		Vector<FieldPair>		  m_fields;
	};

	class GlobalAllocationWrapper;

	struct MetaPair
	{
		TypeID	  tid  = 0;
		MetaType* type = {};
	};

	class ReflectionSystem
	{
	public:
		static inline ReflectionSystem& Get()
		{
			static ReflectionSystem instance;
			return instance;
		}

		template <typename T> MetaType* Meta()
		{
			const TypeID tid = GetTypeID<T>();

			auto it = linatl::find_if(m_metaData.begin(), m_metaData.end(), [tid](MetaPair& pair) -> bool { return tid == pair.tid; });

			if (it == m_metaData.end())
			{
				MetaType* meta = new MetaType();
				m_metaData.push_back({tid, meta});
				return meta;
			}
			return it->type;
		}

		template <typename T> MetaType* Resolve()
		{
			const TypeID tid = GetTypeID<T>();

			auto it = linatl::find_if(m_metaData.begin(), m_metaData.end(), [tid](MetaPair& pair) -> bool { return tid == pair.tid; });

			if (it == m_metaData.end())
			{
				MetaType* meta = new MetaType();
				m_metaData.push_back({tid, meta});
				return meta;
			}
			return it->type;
		}

		MetaType* Resolve(TypeID tid)
		{
			auto it = linatl::find_if(m_metaData.begin(), m_metaData.end(), [tid](MetaPair& pair) -> bool { return tid == pair.tid; });

			if (it == m_metaData.end())
			{
				MetaType* meta = new MetaType();
				m_metaData.push_back({tid, meta});
				return meta;
			}
			return it->type;
		}

		const Vector<MetaPair>& GetTypes() const
		{
			return m_metaData;
		}

	protected:
		void Destroy();

	private:
		bool FindPair(TypeID tid, MetaPair& pair) const
		{
			for (const MetaPair& m : m_metaData)
			{
				if (m.tid == tid)
				{
					pair = m;
					return true;
				}
			}

			return false;
		}

	private:
		friend class GlobalAllocationWrapper;
		friend class Application;

		ReflectionSystem() = default;
		virtual ~ReflectionSystem()
		{
			Destroy();
		}

	private:
		Vector<MetaPair> m_metaData;
	};
} // namespace Lina
