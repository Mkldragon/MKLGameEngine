
#pragma once
#include "ReflectionTest_main.h"
#include "sge_core.h"


class Shape;
template<> const TypeInfo* myTypeOf<Shape>();

class Shape : public Object
{
public:
	virtual const TypeInfo* getType() const override
	{
		return myTypeOf<Shape>();
	}
	float x = 0;
	float y = 0;

};


class Circle;
template<> const TypeInfo* myTypeOf<Circle>();
class Circle : public Shape
{
public:
	virtual const TypeInfo* getType() const override
	{
		return myTypeOf<Circle>();
	}
	float radius = 0;

};

class Rect : public Shape
{
public:
	float width = 0;
	float height = 0;
};



