
#pragma once
#include "sge_core.h"
#include "SgeImGui.h"

namespace sge
{
	class GameObject;

	class Component : public Object
	{
		SGE_TYPE(Component, Object);
	public:
		Component();
		GameObject* gameObject;

	};

	class GameObject : public Object
	{
		SGE_TYPE(GameObject, Object);
	public:
		float	Objtest1 = 0;
		int		Objtest2 = 0;
		void	AddComponent();
		void	GetComponent();
	private:
		Vector<Component, 32> component;
	};



	class ChildComponent : public Component
	{
		SGE_TYPE(ChildComponent, Component);
	public:

		Vec3f	childtest1{0,1,1};
		int		childtest2 = 0;
	};


	


	class GameObjectManager : public NonCopyable
	{
	public:
		static GameObjectManager* instance() { return s_instance; }
		GameObjectManager();
		~GameObjectManager();

		void onGameObjectCreate();
		void AddToList(Object* obj);

		Vector<Object*, 1024> gameObjectData;

	private:
		static GameObjectManager* s_instance;
		GameObject obj;
		Component component;
		ChildComponent childComponent;
	};
}