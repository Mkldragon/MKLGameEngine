#include "RenderGpuBuffer_DX11.h"
#include "Renderer_DX11.h"

namespace sge
{
	RenderGpuBuffer_DX11::RenderGpuBuffer_DX11(CreateDesc& desc) : RenderGpuBuffer(desc)
	{
		if(desc.bufferSize <= 0) throw SGE_ERROR("buffer size = 0");
		if(desc.stride == 0)  throw SGE_ERROR("stride = 0");

		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = DX11Util::castUINT(Math::alignTo(desc.bufferSize, 16));
		bd.StructureByteStride = DX11Util::castUINT(desc.stride);

		switch (desc.type) {
		case Type::Vertex: {
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}break;
		case Type::Index: {
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}break;
		case Type::Const: {
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}break;
		default: throw SGE_ERROR("unsupport gpu buffer type");
		}


		auto* renderer = Renderer_DX11::instance();
		auto* dev = renderer->d3dDevice();

		dev->CreateBuffer(&bd, nullptr, _d3dBuf.ptrForInit());
	}

	void RenderGpuBuffer_DX11::onUploadToGpu(Span<const u8> data, size_t offset)
	{

		auto* renderer = Renderer_DX11::instance();
		auto* ctx = renderer->d3dDeviceContext();
		D3D11_MAPPED_SUBRESOURCE mapped = {};

		ctx->Map(_d3dBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		
		u8* dst = reinterpret_cast<u8*>(mapped.pData);
		memcpy(dst + offset, data.data(), data.size());
		//SGE_LOG("Uploaded to GPU");
		ctx->Unmap(_d3dBuf, 0);
	}

}