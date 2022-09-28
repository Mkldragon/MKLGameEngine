//#pragma once
//#include "sge_core.h"
//
//#define SGE_TYPE(T, BASE)						\
//private:										\
//	using This = T;								\
//	using Base = BASE;							\
//	class TI_Base : public TypeInfoInit<T, BASE> {	\
//	public:										\
//		TI_Base() : TypeInfoInit<T, BASE>(#T) {		\
//		}										\
//	};											\
//public: \
//	static const TypeInfo* s_getType(); \
//	virtual const TypeInfo* getType() const override { return s_getType(); } \
//private:										\
//	
//--------------

//class Object;
//class TypeInfo;
//template<class T> const TypeInfo* myTypeOf();
//
//class TypeManager
//{
//public:
//	static TypeManager* instance();
//	const TypeInfo* find(const char* name)
//	{
//		auto it = m_map.find(name);
//		if (it == m_map.end()) return nullptr;
//		return it->second;
//	}
//	void registerType(const char* name, const TypeInfo* ti)
//	{
//		m_map[name] = ti;
//
//	}
//private:
//	std::map<std::string, const TypeInfo*> m_map;
//
//};
//
//
//template<class OBJ, class FIELD> inline
//intptr_t my_offset(FIELD OBJ::* ptr)
//{
//	OBJ* obj = nullptr;
//	auto* m = &(obj->*ptr);
//	return reinterpret_cast<intptr_t>(m);
//
//}
//
//
//class FieldInfo
//{
//public:
//	template<class OBJ, class FIELD>
//	FieldInfo(const char* _name, FIELD OBJ::* ptr) : 
//			name(_name)
//		,	fieldType(myTypeOf<FIELD>())
//		,	offset(my_offset(ptr))
//	{
//
//	}
//	const char* name = "";
//	const TypeInfo* fieldType = nullptr;
//
//	intptr_t offset = 0;
//};
//
//
//class FieldsEnumerator
//{
//public:
//
//	FieldsEnumerator(const TypeInfo* typeInfo_) : typeInfo(typeInfo_) { }
//
//	class Iterator
//	{
//	public:
//		Iterator(const TypeInfo* typeInfo_, size_t fieldIndex_) :
//			typeInfo(typeInfo_), fieldIndex(fieldIndex_) { }
//
//		bool operator!=(const Iterator& r) const
//		{
//			return typeInfo != r.typeInfo || fieldIndex != r.fieldIndex;
//		}
//		void operator++();
//
//		const FieldInfo& operator*();
//
//	private:
//		const TypeInfo* typeInfo = nullptr;
//		size_t fieldIndex = 0;
//
//	};
//
//	Iterator begin() { return Iterator(typeInfo, 0); }
//
//	Iterator end() { return Iterator(nullptr, 0); }
//
//	const TypeInfo* typeInfo = nullptr;
//};
//
//
//
//class TypeInfo
//{
//public:
//	const char* name = "";
//	const TypeInfo* baseClass = nullptr;
//	const FieldInfo* fieldsArray = nullptr;
//	size_t fieldCount = 0;
//	size_t dataSize = 0;
//
//	FieldsEnumerator fields() const { return FieldsEnumerator(this); }
//
//	//typedef Object* (*Creator)();
//	Object* creator;
//
//
//	Object* createObject() const
//	{
//		if (!creator) return nullptr;
//		return creator;
//	}
//
//	bool isKindOf(const TypeInfo* t) const
//	{
//		auto* p = this;
//		while (p)
//		{
//			if (p == t) return true;
//			p = p->baseClass;
//		}
//		return false;
//	}
//
//	template<class R>
//	bool isKindOf() const { return isKindOf(myTypeOf<R>()); }
//
//
//};
//
//template<class T> const TypeInfo* myTypeOf();
//
//
//class Object;
//template<> const TypeInfo* myTypeOf<Object>();
//
//
//class Object
//{
//public:
//	virtual const TypeInfo* getType() const
//	{
//		return myTypeOf<Object>();
//	}
//};
//
//
//
//inline const TypeInfo* myTypeOf(Object& obj)
//{
//	return obj.getType();
//}
//
//
//template<class DST> inline
//DST* my_cast(Object* src)
//{
//	if (!src)  return nullptr;
//	auto* type = myTypeOf(*src);
//	if (!type) return nullptr;
//	if (!type->isKindOf<DST>()) return nullptr;
//	return static_cast<DST*>(src);
//
//}
//
//
//template<> const TypeInfo* myTypeOf<float>();
//
//
//inline
//void FieldsEnumerator::Iterator::operator++()
//{
//	if (!typeInfo) return;
//
//	fieldIndex++;
//	for (;;)
//	{
//		if (!typeInfo) return;
//		if (fieldIndex < typeInfo->fieldCount) return;
//
//		typeInfo = typeInfo->baseClass;
//		fieldIndex = 0;
//	}
//}
//
//inline const FieldInfo& FieldsEnumerator::Iterator::operator*()
//{
//	return typeInfo->fieldsArray[fieldIndex];
//}
//
//
//
//
//template<class T>
//class TypeInfoInitNoBase : public TypeInfo
//{
//public:
//	TypeInfoInitNoBase(const char* name_)
//	{
//		name = name_;
//		dataSize = sizeof(T);
//	}
//
//	template<size_t N>
//	void setField(const FieldInfo(&fi)[N])
//	{
//		fieldsArray = fi;
//		fieldCount = N;
//	}
//};
//
//template<class T> inline
//static Object* TypeCreator() {
//	return new T();
//}
//
//template<class T, class Base>
//class TypeInfoInit : public TypeInfoInitNoBase<T>
//{
//public:
//	TypeInfoInit(const char* name_) :
//		TypeInfoInitNoBase<T>(name_)
//	{
//		//static_assert(std::is_base_of<Base, T>()::value, "invaild base class");
//		this->baseClass = myTypeOf<Base>();
//		this->creator = TypeCreator<T>();
//	}
//
//	
//};
//
//
//
//
//#define myTypeOf_define(T)						\
//	template<> const TypeInfo* myTypeOf<T>();	\
////----------
//
//myTypeOf_define(float)
//myTypeOf_define(double)
//
//myTypeOf_define(int8_t)
//myTypeOf_define(int16_t)
//myTypeOf_define(int32_t)
//myTypeOf_define(int64_t)
//myTypeOf_define(uint8_t)
//myTypeOf_define(uint16_t)
//myTypeOf_define(uint32_t)
//myTypeOf_define(uint64_t)
//
//myTypeOf_define(char16_t)
//myTypeOf_define(char32_t)
//myTypeOf_define(wchar_t)
//
//template<class T> inline
//const TypeInfo* myTypeOf() {
//	return T::s_getType();
//}


