#include "InspectorUI.h"
#include "sge_engine/GameObject.h"

namespace sge
{
	void RenderTranform(Transform* trans)
	{

		ImGui::Separator();

		ImGui::Text("Transform");
		ImGui::Text("Position");
		ImGui::SameLine(200);
		ImGui::InputFloat3("##dev9", &trans->position[0]);

		ImGui::Text("Rotation");
		ImGui::SameLine(200);
		ImGui::InputFloat3("##dev9", &trans->rotation[0]);

		ImGui::Text("Scale");
		ImGui::SameLine(200);
		ImGui::InputFloat3("##dev9", &trans->localScale[0]);

		ImGui::Separator();

	}


	void InspectorUI::RenderGUI()
	{
		GameObjectManager* gameObjManager = GameObjectManager::instance();
		ImGui::Begin("Inspector");
		ImGui::Separator();
		
		if (gameObjManager->selectedObject != nullptr)
		{
			GameObject* obj = gameObjManager->selectedObject;

			ImGui::Text(obj->getType()->name);


			for (size_t j = 0; j < obj->_component.size(); j++)
			{
				auto* h = obj->_component[j]->getType();

				Transform* transform = my_cast<Transform>(obj->_component[j]);
				if (transform)
					RenderTranform(transform);

				else
				{
					ImGui::Text(h->name);
					for (auto& f : h->fields())
					{
						auto binFileName = Fmt("{}, Type={}, offset={}", f.name, f.fieldType->name, f.offset);
						ImGui::Text(binFileName.c_str());
					}
				}
			}

			if (ImGui::Button("Add Component"))
			{
				gameObjManager->selectedObject->AddComponent<Rigidbody>();
			}
		}


		ImGui::End();
	}

}