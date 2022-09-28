#include "GameObject.h"
#include "sge_core/ECS/TypeReflection.h"

namespace sge
{
	const TypeInfo* GameObject::s_getType()
	{
		class TI : public TI_Base
		{
		public:
			TI()
			{
				static FieldInfo fi[] =
				{
					FieldInfo("Objtest1", &This::Objtest1),
					FieldInfo("Objtest2", &This::Objtest2),
				};
				setField(fi);
			}
		};

		static TI ti;
		return &ti;
	}


	const TypeInfo* Component::s_getType()
	{
		class TI : public TI_Base
		{
		public:
			TI()
			{
				static FieldInfo fi[] =
				{
					FieldInfo("test1", &This::test1),
					FieldInfo("test2", &This::test2),
				};
				setField(fi);
			}
		};

		static TI ti;
		return &ti;
	}

	const TypeInfo* ChildComponent::s_getType()
	{
		class TI : public TI_Base
		{
		public:
			TI()
			{
				static FieldInfo fi[] =
				{
					FieldInfo("childtest1", &This::childtest1),
					FieldInfo("childtest2", &This::childtest2),
				};
				setField(fi);
			}
		};

		static TI ti;
		return &ti;
	}
}