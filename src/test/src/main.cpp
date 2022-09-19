#pragma once
#include "sge_core.h"
#include "ReflectionTest/ReflectionTest_main.h"
#include "ReflectionTest/ReflectTest_TypeA.h"





void TestReflection(Shape& s)
{
	auto* t = myTypeOf(s);
	if (!t)
	{
		std::cout << "unknown Type\n";
	}
	else
	{
		std::cout << t->name << "\n";
	}

	Circle* c = my_cast<Circle>(&s);
	if(c)
		std::cout << "Radius:" << c->radius << "\n";
}

void main()
{
	Circle circle;
	circle.radius = 19;
	TestReflection(circle);

	Rect rect;
	TestReflection(rect);
	
}
