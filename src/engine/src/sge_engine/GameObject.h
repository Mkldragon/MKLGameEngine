
#pragma once
#include "sge_core.h"

namespace sge
{
	class GameObject : public Object
	{
		SGE_TYPE(GameObject, Object);
	public:
		float	Objtest1 = 0;
		int		Objtest2 = 0;

	};

	class Component : public Object
	{
		SGE_TYPE(Component, Object);
	public:
		float	test1 = 0;
		int		test2 = 0;
	};

	class ChildComponent : public Component
	{
		SGE_TYPE(ChildComponent, Component);
	public:
		float	childtest1 = 0;
		int		childtest2 = 0;
	};
}