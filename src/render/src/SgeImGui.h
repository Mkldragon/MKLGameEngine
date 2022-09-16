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
		void SetMaterial(Material* mat);

	private:
		Color4f matColor {1,1,1,1};
		float size = 0;
		//void _createFontTexture();
		int _mouseButton(UIMouseEventButton& event);

		SPtr<Shader>	_shader;
		SPtr<Material>	_material = nullptr;
		SPtr<Texture2D>	_fontTex;

		const VertexLayout* _vertexLayout = nullptr;

		SPtr<RenderGpuBuffer>	_vertexBuffer;
		SPtr<RenderGpuBuffer>	_indexBuffer;

		Vector<u8>	_vertexData;
		Vector<u8>	_indexData;

		ImGuiContext* _ctx = nullptr;
	};
}