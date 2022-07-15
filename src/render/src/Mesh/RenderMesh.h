#pragma once

#include "EditMesh.h"
#include "RenderGpuBuffer.h"
#include "Render_Common.h"
#include "Vertex/VertexLayoutManager.h"

namespace sge {

	class RenderMesh;
	class RenderSubMesh 
	{
	public:
		void create(const EditMesh& src);
		void clear();

		RenderGpuBuffer*	vertexBuffer() const { return constCast(_vertexBuffer); }
		RenderGpuBuffer*	indexBuffer() const  { return constCast(_indexBuffer);  }

		size_t vertexCount() const				{ return _vertexCount; }
		size_t indexCount() const				{ return _indexCount; }
		RenderDataType indexType() const		{ return _indexType; }

		RenderPrimitiveType primitive() const;
		const VertexLayout* vertexLayout() const;

	friend class RenderMesh;
	protected:
		RenderMesh*		_mesh = nullptr;
		RenderDataType _indexType = RenderDataType::None;

		SPtr<RenderGpuBuffer>	_vertexBuffer;
		SPtr<RenderGpuBuffer>	_indexBuffer;

		size_t _vertexCount = 0;
		size_t _indexCount = 0;
	};

	class RenderMesh {
	public:
		void create(const EditMesh& src);
		void clear();

		RenderPrimitiveType primitive() const { return _primitive; }
		const VertexLayout* vertexLayout() const { return _vertexLayout; }

		Span<      RenderSubMesh>	subMeshes()			{ return _subMeshes; }
		Span<const RenderSubMesh>	subMeshes() const	{ return _subMeshes; }
		
		void setSubMeshCount(size_t newSize);

	private:
		RenderPrimitiveType			_primitive = RenderPrimitiveType::Triangles;
		const VertexLayout*			_vertexLayout = nullptr;
		Vector_<RenderSubMesh, 1>	_subMeshes;
	};


	SGE_INLINE RenderPrimitiveType RenderSubMesh::primitive() const {
		return _mesh->primitive();
	}

	SGE_INLINE const VertexLayout* RenderSubMesh::vertexLayout() const {
		return _mesh->vertexLayout();
	}
}