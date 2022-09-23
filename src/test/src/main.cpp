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
	auto* mgr = TypeManager::instance();
	mgr->registerType("Rect", myTypeOf<Rect>());
	auto* ti = TypeManager::instance()->find("Rect");
	Object* obj = ti->createObject();

	Circle circle;
	circle.radius = 19;
	TestReflection(circle);

	Shape shape;
	TestReflection(shape);

	Rect* m_rect = my_cast<Rect>(obj);
	m_rect->width = 5;
	m_rect->height = 10;
	TestReflection(*m_rect);
	
}
