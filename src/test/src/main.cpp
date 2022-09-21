#pragma once
#include "sge_core.h"
#include "ReflectionTest/ReflectionTest_main.h"
#include "ReflectionTest/ReflectTest_TypeA.h"





void TestReflection(Shape& s)
{
	std::cout << "-------------------------------------\n";

	auto* t = myTypeOf(s);
	if (!t)
	{
		std::cout << "unknown Type\n";
		return;
	}

	std::cout << t->name << "\n";
	for (auto& f : t->fields())
	{
		std::cout << f.name << " Type=" << f.fieldType->name << " offset:" << f.offset << "\n";
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

	Shape shape;
	TestReflection(shape);
	
}
