
#pragma once
#include "sge_core.h"
#include "SgeImGui.h"

namespace sge
{
	class GameObject;




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


	
	class HierarchyUI : public ImGuiLayer
	{
	public:
		virtual void RenderGUI() override;

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