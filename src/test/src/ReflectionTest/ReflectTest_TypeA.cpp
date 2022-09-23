#include "ReflectTest_TypeA.h"

const TypeInfo* Shape::s_getType()
{						
	class TI : public TI_Base
	{
		public:
			TI()
			{
				static FieldInfo fi[] = 
				{
					FieldInfo("x", &This::x),
					FieldInfo("y", &This::y)
				};
				setField(fi);
			}
	};

	static TI ti;
	return &ti;
}

const TypeInfo* Circle::s_getType()
{
	class T1 : public TI_Base
	{
	public:
		T1()
		{
			static FieldInfo fi[] = {
				FieldInfo("radius", &This::radius)
			};
			setField(fi);
		}
	};
	static T1 t1;
	return &t1;
}

const TypeInfo* Rect::s_getType()
{
	class TI : public TI_Base
	{
	public:
		TI()
		{
			static FieldInfo fi[] = {
				FieldInfo("width", &This::width),
				FieldInfo("height", &This::height)
			};
			setField(fi);
		}

	};
	static TI ti;
	return &ti;
}


