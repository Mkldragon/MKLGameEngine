#pragma once

class TypeInfo;
template<class T> const TypeInfo* myTypeOf();

class TypeInfo
{
public:
	const char* name;
	const TypeInfo* baseClass = nullptr;

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