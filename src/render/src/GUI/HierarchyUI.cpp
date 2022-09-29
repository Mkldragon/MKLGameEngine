#include "HierarchyUI.h"

namespace sge
{
	void HierarchyUI::RenderGUI()
	{
		ImGui::Begin("Hierarchy");

		String name = "Test";
		for (size_t i = 0; i < 8; i++)
		{
			ImGui::PushID(i);
			String nodeName = name + i;
			bool treeNodeOpen = ImGui::TreeNodeEx(nodeName.c_str(),
													ImGuiTreeNodeFlags_DefaultOpen | 
													ImGuiTreeNodeFlags_FramePadding |
													ImGuiTreeNodeFlags_OpenOnArrow |
													ImGuiTreeNodeFlags_SpanAvailWidth, nodeName.c_str());
			ImGui::PopID();
			if (treeNodeOpen)
			{
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}

}