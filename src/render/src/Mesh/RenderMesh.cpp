#include "RenderMesh.h"
#include <Renderer.h>
#include <Vertex/VertexLayoutManager.h>

namespace sge
{
	struct RenderMesh_InternalHelper {
		template<class DST, class T> inline
			static void copyVertexData(DST* dst
				, size_t vertexCount
				, const VertexLayout::Element& element
				, size_t stride
				, const T* src)
		{
			u8* p = dst + element.offset;

			for (size_t i = 0; i < vertexCount; i++) {
				*reinterpret_cast<T*>(p) = *src;
				src++;
				p += stride;
			}
		}

		static bool hasAttr(size_t arraySize, size_t vertexCount) {
			if (arraySize <= 0) return false;
			if (arraySize < vertexCount) {
				SGE_ASSERT(false);
				return false;
			}
			return true;
		}
	};

	void RenderMesh::create(EditMesh* src)
	{
		_editMesh = src;
		using Helper = RenderMesh_InternalHelper;
		clear();

		u8 uvCount = 0;
		u8 colorCount = 0;
		u8 normalCount = 0;
		u8 tangentCount = 0;
		u8 binormalCount = 0;

		_primitive = RenderPrimitiveType::Triangles;
		size_t vertexCount = src->pos.size();

		if (vertexCount <= 0)
			return;

		if (Helper::hasAttr(src->color.size(), vertexCount)) colorCount = 1;
		if (Helper::hasAttr(src->normal.size(), vertexCount)) normalCount = 1;
		if (Helper::hasAttr(src->tangent.size(), vertexCount)) tangentCount = 1;
		if (Helper::hasAttr(src->binormal.size(), vertexCount)) binormalCount = 1;

		for (u8 i = 0; i < EditMesh::kUvCountMax; i++) {
			if (Helper::hasAttr(src->uv[i].size(), vertexCount)) uvCount = i + 1;
		}

		auto vertexType = VertexTypeUtil::make(
			RenderDataTypeUtil::get<Tuple3f>(),
			RenderDataTypeUtil::get<Color4b>(), colorCount,
			RenderDataTypeUtil::get<Tuple2f>(), uvCount,
			RenderDataTypeUtil::get<Tuple3f>(), normalCount, tangentCount, binormalCount);

		_vertexLayout = VertexLayoutManager::instance()->getLayout(vertexType);

		if (!_vertexLayout) { 
			throw SGE_ERROR("cannot find vertex Layout for mesh");
		}

		setSubMeshCount(1);
		_subMeshes[0].create(*src);

		
	}

	void RenderMesh::UpdateMeshPosition(Vec3f* position, Vec3f* scale)
	{
		if (_editMesh == nullptr) return;

		auto* o_pos = _editMesh->O_pos.begin();
		
		for (auto& pos : _editMesh->pos)
		{
			Vec3f _scale{ 0,0,0 };

			_scale.x = o_pos->x * scale->x;
			_scale.y = o_pos->y * scale->y;
			_scale.z = o_pos->z * scale->z;

			//Mat4f _rot{ _scale.x , _scale.y , _scale.z , 1 };


			pos.x = _scale.x + position->x;
			pos.y = _scale.y + position->y;
			pos.z = _scale.z + position->z;

			o_pos++;
		}

		for (auto& submesh : _subMeshes)
		{
			auto* pData = submesh.vertexData.data();
			auto vc = submesh._vertexCount;

			submesh.uploadToGPU(); 
		}
	}

	void RenderMesh::clear() {
		_vertexLayout = nullptr;
		_subMeshes.clear();
	}


	void RenderSubMesh::create(EditMesh& src) {
		using Helper = RenderMesh_InternalHelper;

		clear();

		_vertexCount = src.pos.size();
		_indexCount = src.index.size();

		auto* vertexLayout = _mesh->vertexLayout();

		if (_vertexCount <= 0)
			return;

		

		
		vertexData.resize(vertexLayout->stride * _vertexCount);
		uploadToGPU();

	}

	void RenderSubMesh::uploadToGPU()
	{
		using Helper = RenderMesh_InternalHelper;

		auto* vertexLayout = _mesh->vertexLayout();
		auto* src = _mesh->editMesh();

		auto* pData = vertexData.data();
		auto stride = vertexLayout->stride;
		auto vc = _vertexCount;

		for (auto& e : vertexLayout->elements) {
			using S = VertexSemantic;
			using ST = VertexSemanticType;
			using U = Vertex_SemanticUtil;

			auto semanticType = U::getType(e.semantic);
			auto semanticIndex = U::getIndex(e.semantic);

			switch (semanticType)
			{
			case ST::TEXCOORD: {
				if (semanticIndex < EditMesh::kUvCountMax) {
					Helper::copyVertexData(pData, vc, e, stride, src->uv[semanticIndex].data()); break;
				}
				continue;
			}
							 break;
			}

			switch (e.semantic) {
			case S::Pos:		Helper::copyVertexData(pData, vc, e, stride, src->pos.data());   break;
			case S::Color0:		Helper::copyVertexData(pData, vc, e, stride, src->color.data()); break;
			case S::Normal:		Helper::copyVertexData(pData, vc, e, stride, src->normal.data()); break;
			case S::Tangent:	Helper::copyVertexData(pData, vc, e, stride, src->tangent.data()); break;
			case S::Binormal:	Helper::copyVertexData(pData, vc, e, stride, src->binormal.data()); break;
			}
		}

		//------
		auto* renderer = Renderer::instance();
		{
			RenderGpuBuffer::CreateDesc desc;
			desc.type = RenderGpuBufferType::Vertex;
			desc.bufferSize = _vertexCount * vertexLayout->stride;
			_vertexBuffer = renderer->createGpuBuffer(desc);
			_vertexBuffer->uploadToGpu(vertexData);
		}

		if (_indexCount > 0) {
			Span<const u8> indexData;
			Vector<u16, 1024> index16Data;

			if (_vertexCount > UINT16_MAX) {
				_indexType = RenderDataType::UInt32;
				indexData = spanCast<const u8, const u32>(src->index);
			}
			else {
				_indexType = RenderDataType::UInt16;
				index16Data.resize(src->index.size());
				for (size_t i = 0; i < src->index.size(); i++) {
					u32 vi = src->index[i];
					index16Data[i] = static_cast<u16>(vi);
				}
				indexData = spanCast<const u8, const u16>(index16Data);
			}

			RenderGpuBuffer::CreateDesc desc;
			desc.type = RenderGpuBufferType::Index;
			desc.bufferSize = indexData.size();
			_indexBuffer = renderer->createGpuBuffer(desc);
			_indexBuffer->uploadToGpu(indexData);
		}
	}

	void RenderSubMesh::clear() {
		_vertexBuffer = nullptr;
		_indexBuffer = nullptr;
		_vertexCount = 0;
		_indexCount = 0;
	}

	void RenderMesh::setSubMeshCount(size_t newSize) {
		size_t oldSize = _subMeshes.size();
		_subMeshes.resize(newSize);
		for (size_t i = oldSize; i < newSize; i++) {
			_subMeshes[i]._mesh = this;
		}
	}
}

