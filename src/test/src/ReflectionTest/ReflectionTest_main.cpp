#include "ReflectionTest_main.h"


#define myTypeOf_impl(T)						\
	template<> const TypeInfo* myTypeOf<T>()	\
	{											\
		static TypeInfoInitNoBase<T> ti(#T);	\
		return &ti;								\
	}											\
//-----

myTypeOf_impl(float)
myTypeOf_impl(double)

myTypeOf_impl(int8_t)
myTypeOf_impl(int16_t)
myTypeOf_impl(int32_t)
myTypeOf_impl(int64_t)

myTypeOf_impl(uint8_t)
myTypeOf_impl(uint16_t)
myTypeOf_impl(uint32_t)
myTypeOf_impl(uint64_t)

myTypeOf_impl(char16_t)
myTypeOf_impl(char32_t)
myTypeOf_impl(wchar_t)


myTypeOf_impl(Object)



