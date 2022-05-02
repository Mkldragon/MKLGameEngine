#pragma once

#include "EditMesh.h"
#include "RenderGpuBuffer.h"
#include "Render_Common.h"
#include "Vertex/VertexLayoutManager.h"

namespace sge {

	class RenderMesh {
	public:
		void create(const EditMesh& src);

		RenderPrimitiveType primitive() const { return _primitive; }
		size_t vertexCount() const { return _vertexCount; }
		RenderGpuBuffer* vertexBuf() const { return _vertexBuf; }
		const VertexLayout* vertexLayout() const { return _vertexLayout; }

	private:

		RenderPrimitiveType _primitive = RenderPrimitiveType::None;
		size_t _vertexCount = 0;
		const VertexLayout* _vertexLayout = nullptr;
		RenderGpuBuffer*	_vertexBuf;
	};

}