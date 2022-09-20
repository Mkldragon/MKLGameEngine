#pragma once
#include "sge_core.h"


class TypeInfo;
template<class T> const TypeInfo* myTypeOf();


template<class OBJ, class FIELD> inline
intptr_t my_offset(FIELD OBJ::* ptr)
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
		,	fieldType(myTypeOf<FIELD>())
		,	offset(my_offset(ptr))
	{

	}
	const char* name = "";
	const TypeInfo* fieldType = nullptr;

	intptr_t offset = 0;
};


class FieldsEnumerator
{
public:

	FieldsEnumerator(const TypeInfo* typeInfo_) : typeInfo(typeInfo_)
	{

	}

		class Iterator
		{
		public:
			Iterator(const TypeInfo* typeInfo_, size_t fieldIndex_) :
				typeInfo(typeInfo_), fieldIndex(fieldIndex_)
			{

			}

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

	Iterator begin()
	{
		return Iterator(typeInfo, 0);
	}

	Iterator end()
	{
		return Iterator(nullptr, 0);
	}


	const TypeInfo* typeInfo = nullptr;
};



class TypeInfo
{
public:
	const char* name;
	const TypeInfo* baseClass = nullptr;

	const FieldInfo* fieldsArray = nullptr;
	size_t fieldCount = 0;

	//sge::Vector<>
	FieldsEnumerator fields() const
	{
		return FieldsEnumerator(this);
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
	bool isKindOf() const
	{
		return isKindOf(myTypeOf<R>());
	}

	template<size_t N>
	void setField(const FieldInfo (&fi)[N])
	{
		fieldsArray = fi;
		fieldCount = N;
	}
};

template<class T> const TypeInfo* myTypeOf();


class Object;
template<> const TypeInfo* myTypeOf<Object>();


class Object
{
public:
	virtual const TypeInfo* getType() const
	{
		return myTypeOf<Object>();
	}
};



inline const TypeInfo* myTypeOf(Object& obj)
{
	return obj.getType();
}


template<class DST> inline
DST* my_cast(Object* src)
{
	if (!src)  return nullptr;
	auto* type = myTypeOf(*src);
	if (!type) return nullptr;
	if (!type->isKindOf<DST>()) return nullptr;
	return static_cast<DST*>(src);

}


template<> const TypeInfo* myTypeOf<float>();


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



