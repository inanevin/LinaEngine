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

#include "Common/Data/String.hpp"
#include "Common/Data/HashMap.hpp"
#include "Common/StringID.hpp"
#include "Common/Data/Functional.hpp"
#include "Common/Data/CommonData.hpp"
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
	public:
		void AddProperty(StringID sid, T p)
		{
			if (m_properties.find(sid) != m_properties.end())
				return;
			m_properties[sid] = p;
			_order.push_back(sid);
		}

		T GetProperty(StringID sid)
		{
			return m_properties[sid];
		}

		bool HasProperty(StringID sid) const
		{
			return m_properties.find(sid) != m_properties.end();
		}

		Vector<T> GetSortedVector()
		{
			Vector<T> vec;
			for (StringID sid : _order)
				vec.push_back(m_properties[sid]);
			return vec;
		}

	private:
		HashMap<StringID, T> m_properties;
		Vector<StringID>	 _order;
	};

	class PropertyCacheManager
	{
	public:
		PropertyCacheManager() = default;

		virtual ~PropertyCacheManager()
		{
		}

		template <typename T> bool HasProperty(StringID sid) const
		{
			const TypeID tid = GetTypeID<T>();
			auto		 it	 = m_propertyCaches.find(tid);
			if (it == m_propertyCaches.end())
				return false;

			return static_cast<PropertyCache<T>*>(it->second)->HasProperty(sid);
		}

		template <typename T> inline void AddProperty(StringID sid, T param)
		{
			const TypeID tid = GetTypeID<T>();
			auto		 it	 = m_propertyCaches.find(tid);
			if (it == m_propertyCaches.end())
			{
				PropertyCache<T>* cache = new PropertyCache<T>();
				m_propertyCaches[tid]	= cache;
				cache->AddProperty(sid, param);
			}
			else
			{
				PropertyCache<T>* cache = static_cast<PropertyCache<T>*>(it->second);
				cache->AddProperty(sid, param);
			}
		}

		template <typename T> inline T GetProperty(StringID sid) const
		{
			const TypeID	   tid	 = GetTypeID<T>();
			PropertyCacheBase* cache = m_propertyCaches.at(tid);
			return static_cast<PropertyCache<T>*>(cache)->GetProperty(sid);
		}

		template <typename T> inline PropertyCache<T>* GetPropertyCache()
		{
			const TypeID	   tid	 = GetTypeID<T>();
			PropertyCacheBase* cache = m_propertyCaches.at(tid);
			return static_cast<PropertyCache<T>*>(cache);
		}

		void Destroy()
		{
			for (auto& [tid, p] : m_propertyCaches)
				delete p;

			m_propertyCaches.clear();
		}

		const HashMap<TypeID, PropertyCacheBase*>& GetPropertyCaches() const
		{
			return m_propertyCaches;
		}

	private:
		HashMap<TypeID, PropertyCacheBase*> m_propertyCaches;
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
	public:
		void AddFunction(StringID sid, Delegate<T>&& f)
		{
			m_functions[sid] = f;
		}

		Delegate<T> GetFunction(StringID sid)
		{
			return m_functions[sid];
		}

		bool HasFunction(StringID sid)
		{
			return m_functions.find(sid) != m_functions.end();
		}

		FunctionCache()			 = default;
		virtual ~FunctionCache() = default;

	private:
		HashMap<StringID, Delegate<T>> m_functions;
	};

	class FieldBase
	{
	public:
		FieldBase() = default;
		virtual ~FieldBase()
		{
			m_propertyCacheManager.Destroy();
		}

		virtual void	   SaveToStream(OStream& stream, void* obj) = 0;
		virtual FieldValue Value(void* obj)							= 0;

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

		const HashMap<TypeID, PropertyCacheBase*>& GetPropertyCaches() const
		{
			return m_propertyCacheManager.GetPropertyCaches();
		}

		template <typename T> inline void AddFunction(StringID sid, Delegate<T>&& del)
		{
			const TypeID tid = GetTypeID<T>();
			auto		 it	 = m_functionCaches.find(tid);
			if (it == m_functionCaches.end())
			{
				FunctionCache<T>* cache = new FunctionCache<T>();
				m_functionCaches[tid]	= cache;
				cache->AddFunction(sid, linatl::move(del));
			}
			else
			{
				FunctionCache<T>* cache = static_cast<FunctionCache<T>*>(it->second);

				if (cache->HasFunction(sid))
					return;
				cache->AddFunction(sid, linatl::move(del));
			}
		}

		template <typename T> inline Delegate<T> GetFunction(StringID sid)
		{
			const TypeID	  tid	= GetTypeID<T>();
			FunctionCache<T>* cache = static_cast<FunctionCache<T>*>(m_functionCaches[tid]);
			return cache->GetFunction(sid);
		}

		inline void AddPositiveDependency(StringID sid, uint8 val)
		{
			m_positiveDependencies[sid] = val;
		}

		inline void AdNegativeDependency(StringID sid, uint8 val)
		{
			m_negativeDependencies[sid] = val;
		}

		const HashMap<StringID, uint8>& GetPositiveDependencies() const
		{
			return m_positiveDependencies;
		}

		const HashMap<StringID, uint8>& GetNegativeDependencies() const
		{
			return m_negativeDependencies;
		}
		int32 _order = 0;

		inline bool* GetFoldValuePtr()
		{
			return &m_foldValue;
		}

	private:
		bool								m_foldValue = false;
		HashMap<StringID, uint8>			m_positiveDependencies;
		HashMap<StringID, uint8>			m_negativeDependencies;
		HashMap<TypeID, FunctionCacheBase*> m_functionCaches;
		PropertyCacheManager				m_propertyCacheManager;
	};

	template <typename T, class C> class Field : public FieldBase
	{
	public:
		Field()			 = default;
		virtual ~Field() = default;

		inline virtual void SaveToStream(OStream& stream, void* obj) override
		{
			T*		 addr = static_cast<T*>(Value(obj).GetPtr());
			const T& val  = *addr;
		}

		inline virtual FieldValue Value(void* obj) override
		{
			FieldValue val;
			val.ptr = &((static_cast<C*>(obj))->*(m_var));
			return val;
		}

		T m_var = T();
	};

	class MetaType
	{
	public:
		inline FieldBase* GetField(StringID sid)
		{
			return m_fields[sid];
		}

		template <auto DATA, typename Class> void AddField(StringID sid)
		{
			Field<decltype(DATA), Class>* f = new Field<decltype(DATA), Class>();
			f->m_var						= DATA;
			f->_order						= static_cast<int32>(m_fields.size());
			m_fields[sid]					= f;
		}

		inline bool HasField(StringID sid) const
		{
			return m_fields.find(sid) != m_fields.end();
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
			auto		 it	 = m_functionCaches.find(tid);
			if (it == m_functionCaches.end())
			{
				FunctionCache<T>* cache = new FunctionCache<T>();
				m_functionCaches[tid]	= cache;
				cache->AddFunction(sid, linatl::move(del));
			}
			else
			{
				FunctionCache<T>* cache = static_cast<FunctionCache<T>*>(it->second);

				if (cache->HasFunction(sid))
					return;
				cache->AddFunction(sid, linatl::move(del));
			}
		}

		template <typename T> inline Delegate<T> GetFunction(StringID sid)
		{
			const TypeID	  tid	= GetTypeID<T>();
			FunctionCache<T>* cache = static_cast<FunctionCache<T>*>(m_functionCaches[tid]);
			return cache->GetFunction(sid);
		}

		const HashMap<StringID, FieldBase*>& GetFields() const
		{
			return m_fields;
		}

		Vector<FieldBase*> GetFieldsOrdered() const
		{
			Vector<FieldBase*> fields;
			fields.reserve(m_fields.size());

			for (auto [sid, fb] : m_fields)
				fields.push_back(fb);

			linatl::sort(fields.begin(), fields.end(), [](FieldBase* f1, FieldBase* f2) -> bool { return f1->_order < f2->_order; });
			return fields;
		}

		inline PropertyCacheManager& GetPropertyCacheManager()
		{
			return m_propertyCacheManager;
		}

	private:
		friend class ReflectionSystem;
		PropertyCacheManager				m_propertyCacheManager;
		HashMap<TypeID, FunctionCacheBase*> m_functionCaches;
		HashMap<StringID, FieldBase*>		m_fields;
	};

	class GlobalAllocationWrapper;

	class ReflectionSystem
	{
	public:
		static inline ReflectionSystem& Get()
		{
			static ReflectionSystem instance;
			return instance;
		}

		template <typename T> MetaType& Meta()
		{
			const TypeID tid = GetTypeID<T>();
			return m_metaData[tid];
		}

		template <typename T> MetaType& Resolve()
		{
			const TypeID tid = GetTypeID<T>();
			return m_metaData[tid];
		}

		MetaType& Resolve(TypeID tid)
		{
			LINA_ASSERT(m_metaData.find(tid) != m_metaData.end(), "");
			return m_metaData[tid];
		}

		const HashMap<TypeID, MetaType>& GetTypes() const
		{
			return m_metaData;
		}

	protected:
		void Destroy();

	private:
		friend class GlobalAllocationWrapper;
		friend class Application;

		ReflectionSystem() = default;
		virtual ~ReflectionSystem()
		{
			Destroy();
		}

	private:
		HashMap<TypeID, MetaType> m_metaData;
	};
} // namespace Lina
