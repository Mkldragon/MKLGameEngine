#include "SgeImGui.h"
#include "RenderContext.h"
#include "RenderRequest.h"
#include "Renderer.h"

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"


namespace sge
{
	ImGui_Sge::~ImGui_Sge()
	{
		destory();
	}
	void ImGui_Sge::create(CreateDesc& desc, Renderer_DX11* renderer)
	{
		if(!IMGUI_CHECKVERSION())
			throw SGE_ERROR("ImGui version error");
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		ImGui_ImplWin32_Init(desc.window->_hwnd);
		ImGui_ImplDX11_Init(renderer->d3dDevice(), renderer->d3dDeviceContext());

	}

	void ImGui_Sge::destory()
	{
		if (!_ctx) return;

		ImGuiIO& io = ImGui::GetIO();
		io.BackendRendererUserData = nullptr;
		io.BackendRendererName = nullptr;

		ImGui::DestroyContext(_ctx);
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		_ctx = nullptr;
	}

	void ImGui_Sge::onBeginRender(RenderContext* renderCtx)
	{

	}

	void ImGui_Sge::onEndRender(RenderContext* renderCtx)
	{

	}

	void ImGui_Sge::onDrawUI()
	{

		if (_guiRenderLayers.size() == 0) return;


		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();



		auto layer = *_guiRenderLayers.begin();
		int size = 0;

		for (size_t i = 0; i < _guiRenderLayers.size(); i++)
		{
			_guiRenderLayers[i]->RenderGUI();
		}

		//ImGui::Begin("Hierarchy");

		//String name = "Test";
		//for (size_t i = 0; i < 8; i++)
		//{
		//	ImGui::PushID(i);
		//	String nodeName = name + i;
		//	bool treeNodeOpen = ImGui::TreeNodeEx(nodeName.c_str(),
		//											ImGuiTreeNodeFlags_DefaultOpen | 
		//											ImGuiTreeNodeFlags_FramePadding |
		//											ImGuiTreeNodeFlags_OpenOnArrow |
		//											ImGuiTreeNodeFlags_SpanAvailWidth, nodeName.c_str());
		//	ImGui::PopID();
		//	if (treeNodeOpen)
		//	{
		//		ImGui::TreePop();
		//	}
		//}

		//ImGui::End();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	}
	void ImGui_Sge::onUIMouseEvent(UIMouseEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		using Type = UIMouseEventType;

		switch (event.type)
		{
			case Type::Move: {
				io.AddMousePosEvent(event.pos.x, event.pos.y);
			} break;

			case Type::Down: {
				io.AddMouseButtonEvent(_mouseButton(event.pressedButtons), true);
			} break;

			case Type::Up: {
				io.AddMouseButtonEvent(_mouseButton(event.pressedButtons), false);
			} break;

			case Type::Scroll: {
				io.AddMouseWheelEvent(event.scroll.x, event.scroll.y);
			} break;
		}

		event.isCaptureImGui = io.WantCaptureMouse;
	}

	void ImGui_Sge::registerLayer(ImGuiLayer* layer)
	{
		
		_guiRenderLayers.emplace_back(layer);
	}

	void ImGui_Sge::unRegisterLayer(ImGuiLayer* layer)
	{
		
	}


	int ImGui_Sge::_mouseButton(UIMouseEventButton& event)
	{
		using Button = UIMouseEventButton;
		switch (event) {
		case Button::Left:		return 0;
		case Button::Right:		return 1;
		case Button::Middle:	return 2;
		case Button::Button4:	return 3;
		case Button::Button5:	return 4;
		}
		return 0;
	}

	ImGuiLayer::ImGuiLayer()
	{

	}
}
