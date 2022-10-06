#pragma once

#include "sge_core.h"
#include "SgeImGui.h"

namespace sge
{
	class InspectorUI : public ImGuiLayer
	{
	public:
		virtual void RenderGUI() override;

	};

}