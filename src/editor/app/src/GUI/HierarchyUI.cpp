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
			ImGui::PushID(_id);
			GameObject* obj = my_cast<GameObject>(gameObjManager->gameObjectData[i].ptr());
			auto* t = obj->getType();
			String nodeName = obj->name.c_str();
			Transform* trans = obj->GetComponent<Transform>();

			if (trans->parent != nullptr)
			{
				ImGui::PopID();
				continue;
			}


			if (trans->getChildCount() == 0)
			{
				ImGui::Selectable(nodeName.c_str(), false);
				ImGui::PopID();
				if (ImGui::IsItemClicked()) gameObjManager->SelectHirearchyObject(obj);
				continue;
			}
	
			if (ImGui::IsItemClicked()) gameObjManager->SelectHirearchyObject(obj);

			bool treeNodeOpen = ImGui::TreeNodeEx(nodeName.c_str(),
				ImGuiTreeNodeFlags_FramePadding |
				ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_SpanAvailWidth, nodeName.c_str());

			ImGui::PopID();
			if (treeNodeOpen)
			{
				ImGui::TreePop();
				ImGui::Separator();
			}
			_id++;
		}


		ImGui::End();

	}

	void HierarchyUI::ChildGameObjectHandle(Vector<Transform*, 64>* children)
	{
		for (auto* i = children->begin(); i != children->end(); i++) {
			std::cout << i << " ";
		}
	}


}