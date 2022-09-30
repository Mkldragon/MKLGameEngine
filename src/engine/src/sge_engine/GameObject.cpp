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
					FieldInfo("test1", &This::test1),
					FieldInfo("test2", &This::test2),
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

	void GameObjectManager::onGameObjectCreate()
	{
		obj.Objtest1 = 1;
		obj.Objtest2 = 2;

		component.test1 = 3;
		component.test2 = 4;


		AddToList(&obj);
		AddToList(&component);
		AddToList(&childComponent);

	}

	void GameObjectManager::AddToList(Object* obj)
	{
		gameObjectData.emplace_back(obj);
	}
	void HierarchyUI::RenderGUI()
	{
		GameObjectManager* gameObjManager = GameObjectManager::instance();

		if (gameObjManager->gameObjectData.size() == 0) return;

		ImGui::Begin("Hierarchy");
		for (size_t i = 0; i < gameObjManager->gameObjectData.size(); i++)
		{
			ImGui::PushID(i);
			auto* t = gameObjManager->gameObjectData[i]->getType();
			String nodeName = t->name;
			bool treeNodeOpen = ImGui::TreeNodeEx(nodeName.c_str(),
				ImGuiTreeNodeFlags_FramePadding |
				ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_SpanAvailWidth, nodeName.c_str());

			ImGui::PopID();
			if (treeNodeOpen)	
			{
				for (auto& f : t->fields())
				{
					auto binFileName = Fmt("{}, Type={}, offset={}", f.name, f.fieldType->name, f.offset);
					ImGui::Text(binFileName.c_str());
				}
				ImGui::TreePop();
				ImGui::Separator();
			}
		}
		ImGui::End();
		
	}
}