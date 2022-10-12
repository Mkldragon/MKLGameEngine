#include "HierarchyUI.h"
#include "sge_engine/GameObject.h"

namespace sge
{


	void HierarchyUI::RenderGUI()
	{
		_id = 0;
		GameObjectManager* gameObjManager = GameObjectManager::instance();

		if (gameObjManager->gameObjectData.size() == 0) return;

		ImGui::Begin("Hierarchy");

		for (size_t i = 0; i < gameObjManager->gameObjectData.size(); i++)
		{
			GameObject* obj = my_cast<GameObject>(gameObjManager->gameObjectData[i].ptr());
			auto* t = obj->getType();
			String nodeName = obj->name.c_str();
			Transform* trans = obj->GetComponent<Transform>();
			if (trans->parent != nullptr) continue;

			ChildGameObjectHandle(trans);

		}


		ImGui::End();

	}

	void HierarchyUI::ChildGameObjectHandle(Transform* transform)
	{
		GameObjectManager* gameObjManager = GameObjectManager::instance();

		const char* objname = transform->gameObject->name.c_str();
		bool treeNodeOpen = ImGui::TreeNodeEx(objname,
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_OpenOnArrow |
			(selected ? ImGuiTreeNodeFlags_Selected : 0) |
			(transform->getChildCount() == 0 ? ImGuiTreeNodeFlags_Leaf : 0), objname);

		ImGui::PushID(_id);
		if (ImGui::BeginPopupContextItem()) {
			ImGui::EndPopup();
		}
		ImGui::PopID();

		if (ImGui::IsItemClicked()) {
			gameObjManager->SelectHirearchyObject(transform->gameObject);
		}

		if (treeNodeOpen)
		{
			int i = 0;
			while (transform->getChild(i) != nullptr)
			{
				ChildGameObjectHandle(transform->getChild(i));
				i++;
			}
			ImGui::TreePop();
			ImGui::Separator();
		}
		_id++;


	}


}