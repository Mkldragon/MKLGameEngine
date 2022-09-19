#include "ReflectionTest_main.h"

template<> const TypeInfo* myTypeOf<Object>()
{
	class T1 : public TypeInfo
	{
	public:
		T1()
		{
			name = "Object";
		}
	};

	static T1 t1;
	return &t1;
}