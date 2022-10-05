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
					FieldInfo("componet1", &This::componet1),
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


	const TypeInfo* Rigidbody::s_getType()
	{
		class TI : public TI_Base
		{
		public:
			TI()
			{
				static FieldInfo fi[] =
				{
					FieldInfo("childtest1", &This::child2test1),
					FieldInfo("childtest2", &This::child2test2),
					FieldInfo("test4", &This::test4),
				};
				setField(fi);
			}
		};

		static TI ti;
		return &ti;
	}




	GameObjectManager* GameObjectManager::s_instance = nullptr;
	GameObjectManager::GameObjectManager()
	{
		SGE_ASSERT(s_instance == nullptr);
		s_instance = this;
	}

	GameObjectManager::~GameObjectManager()
	{
		SGE_ASSERT(s_instance == this);
		s_instance = nullptr;
	}

	void GameObjectManager::AddToList(Object* obj)
	{
		gameObjectData.emplace_back(obj);
	}


	

}