#include "HierarchyUI.h"
#include "sge_engine/GameObject.h"

namespace sge
{
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

				GameObject* obj = my_cast<GameObject>(gameObjManager->gameObjectData[i].ptr());
				if (obj)
				{
					if (obj->_component.size() == 0) break;


					for (size_t j = 0; j < obj->_component.size(); j++)
					{
						auto* h = obj->_component[j]->getType();
						ImGui::Text(h->name);
						for (auto& f : h->fields())
						{
							auto binFileName = Fmt("{}, Type={}, offset={}", f.name, f.fieldType->name, f.offset);
							ImGui::Text(binFileName.c_str());
						}

					}
				}


				ImGui::TreePop();
				ImGui::Separator();
			}
		}
		ImGui::End();

	}
}