#include "InspectorUI.h"
#include "sge_engine/GameObject.h"

namespace sge
{
	void TranformGUI(Transform* trans)
	{

		ImGui::Separator();

		ImGui::Text("Transform");
		ImGui::SameLine(90);
		ImGui::Button("Remove");
		ImGui::SameLine(150);
		ImGui::Button("Reset");

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

	void BoxColliderGUI(BoxCollider* boxCollider)
	{
		ImGui::Separator();
		ImGui::Text("BoxCollider");
		ImGui::Text("IsTrigger");
		ImGui::SameLine(200);
		ImGui::Checkbox("##dev9", &boxCollider->isTrigger);

		ImGui::Text("Center");
		ImGui::SameLine(200);
		ImGui::InputFloat3("##dev9", &boxCollider->center[0]);

		ImGui::Text("Size");
		ImGui::SameLine(200);
		ImGui::InputFloat3("##dev9", &boxCollider->size[0]);

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
			ImGui::Text(obj->name.c_str());


			for (size_t j = 0; j < obj->_component.size(); j++)
			{
				auto* h = obj->_component[j]->getType();

				
				Transform* transform = my_cast<Transform>(obj->_component[j]);
				BoxCollider* boxCollider = my_cast<BoxCollider>(obj->_component[j]);

				if (transform)
					TranformGUI(transform);
				

				else if (boxCollider)
					BoxColliderGUI(boxCollider);

			}

			if (ImGui::Button("Add Component"))
			{
				gameObjManager->selectedObject->AddComponent<Rigidbody>();
			}
		}


		ImGui::End();
	}

}