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


		ImGui::DragFloat3("Position", &trans->position[0], 0.01f, -200.0f, 100.0f);
		ImGui::DragFloat3("Rotation", &trans->rotation[0], 0.01f, -200.0f, 100.0f);
		ImGui::DragFloat3("Scale", &trans->localScale[0], 0.01f, -200.0f, 100.0f);

		ImGui::Separator();

	}

	void BoxColliderGUI(BoxCollider* boxCollider)
	{
		ImGui::Separator();
		ImGui::Text("BoxCollider");
		ImGui::Text("IsTrigger");
		ImGui::SameLine(200);
		ImGui::Checkbox("##dev9", &boxCollider->isTrigger);


		ImGui::DragFloat3("Center", &boxCollider->center[0], 0.01f, -200.0f, 100.0f);
		ImGui::DragFloat3("Size", &boxCollider->size[0], 0.01f, -200.0f, 100.0f);


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
				else
				{
					ImGui::Separator();
					ImGui::Text(h->name);
					ImGui::Separator();
				}

			}

			if (ImGui::Button("Add Component"))
			{
				gameObjManager->selectedObject->AddComponent<Rigidbody>();
			}

			if (ImGui::Button("ShowLog"))
			{
				GameObject* g = gameObjManager->selectedObject;
				std::cout << g->transform->position.x << " " << g->transform->position.y << " " << g->transform->position.z << "\n";

				std::cout << g->transform->rotation.x << " " << g->transform->rotation.y << " " << g->transform->rotation.z << "\n";
				std::cout << g->transform->localScale.x << " " << g->transform->localScale.y << " " << g->transform->localScale.z << "\n";
			}
		}


		ImGui::End();
	}

}