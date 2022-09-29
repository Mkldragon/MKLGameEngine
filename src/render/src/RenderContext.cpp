#include "RenderContext.h"
#include "Renderer.h"

namespace sge
{
	void RenderContext::beginRender()
	{
		_imgui.onBeginRender(this);
		onBeginRender();
	}
	void RenderContext::endRender()
	{
		onEndRender();
		_imgui.onEndRender(this);
	}


	void RenderContext::setFrameBufferSize(Vec2f newSize) {
		if (_frameBufferSize == newSize)
			return;

		_frameBufferSize = newSize;
		onSetFrameBufferSize(newSize);
	}


	RenderContext::RenderContext(CreateDesc& desc) {}

	void RenderContext::onPostCreate(CreateDesc& desc)
	{
		
	}

	void RenderContext::drawUI()
	{
		_imgui.onDrawUI();
	}

	void RenderContext::onUIMouseEvent(UIMouseEvent& event)
	{
		_imgui.onUIMouseEvent(event);
	}



	

}
