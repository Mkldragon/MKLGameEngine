#pragma once

#include "sge_core.h"
#include "SgeImGui.h"

namespace sge
{
	class HierarchyUI : public ImGuiLayer
	{
	public:
		virtual void RenderGUI() override;

	};

}