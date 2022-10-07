#pragma once
#include "sge_core.h"

namespace sge
{

	class Object;
	class TypeInfo;

	template<class T> const TypeInfo* typeOf();

	class TypeManager
	{
	public:
		static TypeManager* instance();
		const TypeInfo* find(const char* name)
		{
			auto it = m_map.find(name);
			if (it == m_map.end()) return nullptr;
			return it->second;
		}
		void registerType(const char* name, const TypeInfo* ti)
		{
			m_map[name] = ti;

		}
	private:
		Map<eastl::string, const TypeInfo*> m_map;

	};


	template<class OBJ, class FIELD> inline
		intptr_t type_offset(FIELD OBJ::* ptr)
	{
		OBJ* obj = nullptr;
		auto* m = &(obj->*ptr);
		return reinterpret_cast<intptr_t>(m);

	}


	class FieldInfo
	{
	public:
		template<class OBJ, class FIELD>
		FieldInfo(const char* _name, FIELD OBJ::* ptr) :
			name(_name)
			, fieldType(typeOf<FIELD>())
			, offset(type_offset(ptr))
		{

		}
		const char* name = "";
		const TypeInfo* fieldType = nullptr;

		intptr_t offset = 0;
	};


	class FieldsEnumerator
	{
	public:

		FieldsEnumerator(const TypeInfo* typeInfo_) : typeInfo(typeInfo_) { }

		class Iterator
		{
		public:
			Iterator(const TypeInfo* typeInfo_, size_t fieldIndex_) :
				typeInfo(typeInfo_), fieldIndex(fieldIndex_) { }

			bool operator!=(const Iterator& r) const
			{
				return typeInfo != r.typeInfo || fieldIndex != r.fieldIndex;
			}
			void operator++();

			const FieldInfo& operator*();

		private:
			const TypeInfo* typeInfo = nullptr;
			size_t fieldIndex = 0;

		};

		Iterator begin() { return Iterator(typeInfo, 0); }

		Iterator end() { return Iterator(nullptr, 0); }

		const TypeInfo* typeInfo = nullptr;
	};



	class TypeInfo
	{
	public:
		const char* name = "";
		const TypeInfo* baseClass = nullptr;
		const FieldInfo* fieldsArray = nullptr;
		size_t fieldCount = 0;
		size_t dataSize = 0;

		FieldsEnumerator fields() const { return FieldsEnumerator(this); }

		//typedef Object* (*Creator)();
		Object* creator;


		Object* createObject() const
		{
			if (!creator) return nullptr;
			return creator;
		}

		bool isKindOf(const TypeInfo* t) const
		{
			auto* p = this;
			while (p)
			{
				if (p == t) return true;
				p = p->baseClass;
			}
			return false;
		}

		template<class R>
		bool isKindOf() const { return isKindOf(typeOf<R>()); }


	};

	template<class T> const TypeInfo* typeOf();


	class Object;
	template<> const TypeInfo* typeOf<Object>();


	class Object : public RefCountBase
	{
	public:
		Object() = default;
		virtual const TypeInfo* getType() const
		{
			return typeOf<Object>();
		}
	private:
		Object(const Object&) = delete;
		void operator=(const Object&) = delete;
	};



	inline const TypeInfo* typeOf(Object& obj)
	{
		return obj.getType();
	}


	template<class DST> inline
		DST* my_cast(Object* src)
	{
		if (!src)  return nullptr;
		auto* type = typeOf(*src);
		if (!type) return nullptr;
		if (!type->isKindOf<DST>()) return nullptr;
		return static_cast<DST*>(src);

	}


	template<> const TypeInfo* typeOf<float>();


	inline
		void FieldsEnumerator::Iterator::operator++()
	{
		if (!typeInfo) return;

		fieldIndex++;
		for (;;)
		{
			if (!typeInfo) return;
			if (fieldIndex < typeInfo->fieldCount) return;

			typeInfo = typeInfo->baseClass;
			fieldIndex = 0;
		}
	}

	inline const FieldInfo& FieldsEnumerator::Iterator::operator*()
	{
		return typeInfo->fieldsArray[fieldIndex];
	}




	template<class T>
	class TypeInfoInitNoBase : public TypeInfo
	{
	public:
		TypeInfoInitNoBase(const char* name_)
		{
			name = name_;
			dataSize = sizeof(T);
		}

		template<size_t N>
		void setField(const FieldInfo(&fi)[N])
		{
			fieldsArray = fi;
			fieldCount = N;
		}
	};

	template<class T> inline
		static Object* TypeCreator() {
		return new T();
	}

	template<class T, class Base>
	class TypeInfoInit : public TypeInfoInitNoBase<T>
	{
	public:
		TypeInfoInit(const char* name_) :
			TypeInfoInitNoBase<T>(name_)
		{
			//static_assert(std::is_base_of<Base, T>()::value, "invaild base class");
			this->baseClass = typeOf<Base>();
			this->creator = TypeCreator<T>();
		}


	};

//----------

	SGE_TYPE_DEFINE(float)
	SGE_TYPE_DEFINE(double)
	SGE_TYPE_DEFINE(bool)

	SGE_TYPE_DEFINE(int8_t)
	SGE_TYPE_DEFINE(int16_t)
	SGE_TYPE_DEFINE(int32_t)
	SGE_TYPE_DEFINE(int64_t)
	SGE_TYPE_DEFINE(uint8_t)
	SGE_TYPE_DEFINE(uint16_t)
	SGE_TYPE_DEFINE(uint32_t)
	SGE_TYPE_DEFINE(uint64_t)
	SGE_TYPE_DEFINE(Vec3f)
	SGE_TYPE_DEFINE(Vec2d)
	SGE_TYPE_DEFINE(Tuple3f)
	SGE_TYPE_DEFINE(String)

	SGE_TYPE_DEFINE(char16_t)
	SGE_TYPE_DEFINE(char32_t)
	SGE_TYPE_DEFINE(wchar_t)

	template<class T> inline
	const TypeInfo* typeOf() 
	{
		return T::s_getType();
	}

}