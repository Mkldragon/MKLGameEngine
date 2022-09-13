#pragma once

#include "sge_core.h"
#include "RenderCommand.h"
#include "SgeImGui.h"

namespace sge {

	class RenderCommandBuffer;
	class RenderRequest;

	struct RenderContext_CreateDesc {
		NativeUIWindow* window = nullptr;
	};


	class RenderContext : public Object
	{
	public:
		using CreateDesc = RenderContext_CreateDesc;


		void beginRender();
		void endRender();


		void setFrameBufferSize(Vec2f newSize);
		const Vec2f& frameBufferSize() const { return _frameBufferSize; }

		void commit(RenderCommandBuffer& cmdBuf) { onCommit(cmdBuf); }


		RenderContext(CreateDesc& desc);
		virtual ~RenderContext() = default;
		
		void onPostCreate(CreateDesc& desc);
		void drawUI();
		void onUIMouseEvent(UIMouseEvent& event);


	protected:

		virtual void onBeginRender() {};
		virtual void onEndRender() {};

		virtual void onCommit(RenderCommandBuffer& cmdBuf) {}

		virtual void onSetFrameBufferSize(Vec2f newSize) {};


		Vec2f	_frameBufferSize{ 0,0 };

		template<class IMPL>
		void _dispatch(IMPL* impl, RenderCommandBuffer& cmdBuf)
		{
			using Cmd = RenderCommandType;

		#define CMD_CASE(E) \
			case Cmd::E: { \
				auto* c = static_cast<RenderCommand_##E*>(cmd); \
				impl->onCmd_##E(*c); \
			} break; \
		//----

			for (auto* cmd : cmdBuf.commands()) {
				switch (cmd->type()) {
					CMD_CASE(ClearFrameBuffers)
						CMD_CASE(SwapBuffers);
					CMD_CASE(DrawCall);
				default:
					throw SGE_ERROR("unhandled command");
				}
			}

		#undef CMD_CASE
		}
		ImGui_Sge _imgui;
	};


}