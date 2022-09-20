#include "ReflectTest_TypeA.h"

template<> const TypeInfo* myTypeOf<Shape>()
{
	class T1 : public TypeInfo
	{
	public:
		T1()
		{
			name = "Shape";
			baseClass = myTypeOf<Object>();
		

			static FieldInfo fi[] = {
				FieldInfo("x", &Shape::x),
				FieldInfo("y", &Shape::y)
			};
			setField(fi);
		}


	};

	static T1 t1;
	return &t1;
}

template<> const TypeInfo* myTypeOf<Circle>()
{
	class T1 : public TypeInfo
	{
	public:
		T1()
		{
			name = "Circle";
			baseClass = myTypeOf<Shape>();


			static FieldInfo fi[] = {
				FieldInfo("radius", &Circle::radius)
			};

			setField(fi);
		}

	};

	static T1 t1;
	return &t1;
}


