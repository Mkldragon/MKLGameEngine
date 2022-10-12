#pragma once

#include "sge_core.h"
#include "SgeImGui.h"
#include "sge_engine/GameObject.h"

namespace sge
{
	class HierarchyUI : public ImGuiLayer
	{
	public:
		virtual void RenderGUI() override;
	private:
		void ChildGameObjectHandle(Transform* transform);
		int _id = 0;

		int selected = 0;
	};

}