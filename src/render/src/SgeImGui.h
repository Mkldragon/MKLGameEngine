#pragma once

#include <imgui.h>
#include "sge_core.h"
#include <texture/Texture.h>
#include <Shader/Material.h>
#include "DX11/Renderer_DX11.h"

namespace sge
{
	class RenderContext;
	class RenderRequest;
	class ImGuiLayer;


	class ImGuiLayer : public NonCopyable
	{
	public:
		virtual void RenderGUI() {};

	};

	class ImGui_Sge : public NonCopyable
	{
	public:
		using CreateDesc = RenderContext_CreateDesc;

		~ImGui_Sge();

		void create(CreateDesc& desc, Renderer_DX11* renderer);
		void destory();

		void onBeginRender(RenderContext* renderCtx);
		void onEndRender(RenderContext* renderCtx);

		void onDrawUI();
		void onUIMouseEvent(UIMouseEvent& event);

		void registerLayer(ImGuiLayer* layer);
		void unRegisterLayer(ImGuiLayer* layer);

	private:
		Color4f matColor {1,1,1,1};
		float size = 0;
		//void _createFontTexture();
		int _mouseButton(UIMouseEventButton& event);

		SPtr<Shader>	_shader;
		SPtr<Texture2D>	_fontTex;

		const VertexLayout* _vertexLayout = nullptr;

		SPtr<RenderGpuBuffer>	_vertexBuffer;
		SPtr<RenderGpuBuffer>	_indexBuffer;

		Vector<u8>			_vertexData;
		Vector<u8>			_indexData;
		Vector<ImGuiLayer>  _guiRenderLayers;

		ImGuiContext* _ctx = nullptr;
	};
}