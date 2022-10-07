
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



	class Transform : public Component
	{
		SGE_TYPE(Transform, Component);
	public:
		Tuple3f position { 0, 0, 0 };
		Tuple3f rotation { 0, 0, 0 };
		Tuple3f localScale{ 0, 0, 0 };

	};

	class Collider : public Component
	{
		SGE_TYPE(Collider, Component);
	public:
		bool isTrigger = false;

	};

	class BoxCollider : public Collider
	{
		SGE_TYPE(BoxCollider, Collider);
	public:
		Tuple3f center{ 0, 0, 0 };
		Tuple3f size{ 0,0,0 };

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
		String		name = "GameObject";
		float		Objtest1 = 0;
		int			Objtest2 = 0;

		void AddComponentToObj (Component* component) 
		{
			component->gameObject = this;
			this->_component.emplace_back(component);
		}

		template<class T>
		void AddComponent()
		{ 
			this->AddComponentToObj(new T());
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
		void SelectHirearchyObject(GameObject* obj);
		Vector<SPtr<Object> , 1024> gameObjectData;
		GameObject* selectedObject = nullptr;
	private:
		static GameObjectManager* s_instance;
		Component component;
		Transform childComponent;
	};


}