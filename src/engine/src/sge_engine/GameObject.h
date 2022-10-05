
#pragma once
#include "sge_core.h"
#include "SgeImGui.h"

namespace sge
{
	class GameObject;
	class Component;

	class Component : public Object
	{
		SGE_TYPE(Component, Object);
	public:
		Component() = default;
		int componet1;
		GameObject* gameObject;

	};


	class ChildComponent : public Component
	{
		SGE_TYPE(ChildComponent, Component);
	public:

		Vec3f	childtest1{ 0,1,1 };
		int		childtest2 = 0;
	};



	class Rigidbody : public Component
	{
		SGE_TYPE(Rigidbody, Component);
	public:

		Vec3f	child2test1{ 0,1,1 };
		int		child2test2 = 0;
		float	test4 = 10;
	};




	class GameObject : public Object
	{
		SGE_TYPE(GameObject, Object);
	public:
		float		Objtest1 = 0;
		int			Objtest2 = 0;

		void AddComponentToObj (Component* component) 
		{
			this->_component.emplace_back(component);
		}

		template<class T>
		void AddComponent(T* t)
		{ 
			this->AddComponentToObj(t);
		}
		Vector<SPtr<Component>, 32> _component;
	private:
	};



	class GameObjectManager : public NonCopyable
	{
	public:
		static GameObjectManager* instance() { return s_instance; }
		GameObjectManager();
		~GameObjectManager();

		void CreateGameObject();
		void AddToList(Object* obj);

		Vector<SPtr<Object> , 1024> gameObjectData;

	private:
		static GameObjectManager* s_instance;
		Component component;
		ChildComponent childComponent;
	};


}