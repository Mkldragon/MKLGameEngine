#include "Material_DX11.h"
#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"

namespace sge
{
	void Material_DX11::MyVertexStage::bind(RenderContext_DX11* ctx, const VertexLayout* vertexLayout)
	{
		_bindStageHelper(ctx, this);
		bindInputLayout(ctx, vertexLayout);
	}

	void Material_DX11::MyPixelStage::bind(RenderContext_DX11* ctx, const VertexLayout* vertexLayout)
	{
		_bindStageHelper(ctx, this);
	}

	void Material_DX11::MyVertexStage::bindInputLayout(RenderContext_DX11* ctx, const VertexLayout* vertexLayout)
	{
		if (!vertexLayout) throw SGE_ERROR("VertexLayer is null");

		auto* dev = ctx->renderer()->d3dDevice();
		auto* dc = ctx->renderer()->d3dDeviceContext();

		ID3D11InputLayout* dxLayout = nullptr;

		auto it = _inputLayoutsMap.find(vertexLayout);

		if (it != _inputLayoutsMap.end())
		{
			dxLayout = it->second;
		}
		else
		{
			Vector_<D3D11_INPUT_ELEMENT_DESC, 32> inputDesc;

			for (auto& e : vertexLayout->elements)
			{
				auto& dst = inputDesc.emplace_back();
				auto semanticType = Vertex_SemanticUtil::getType(e.semantic);
				dst.SemanticName = DX11Util::getDxSemanticName(semanticType);
				dst.SemanticIndex = Vertex_SemanticUtil::getIndex(e.semantic);
				dst.Format = DX11Util::getDxFormat(e.dataType);
				dst.InputSlot = 0;
				dst.AlignedByteOffset = e.offset;
				dst.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				dst.InstanceDataStepRate = 0;
			}

			ComPtr<ID3D11InputLayout> outLayout;

			auto* ss = static_cast<Shader_DX11::MyVertexStage*>(_shaderStage);
			auto bytecode = ss->bytecode();

			auto hr = dev->CreateInputLayout(inputDesc.data()
				, static_cast<UINT>(inputDesc.size())
				, bytecode.data()
				, bytecode.size()
				, outLayout.ptrForInit());

			DX11Util::throwIfError(hr);
			
			dxLayout = outLayout;
			_inputLayoutsMap[vertexLayout] = std::move(outLayout);

		}

		dc->IASetInputLayout(dxLayout);

	}



	Material_DX11::MyPass::MyPass(Material* material, ShaderPass* shaderPass) : MaterialPass(material, shaderPass)
		, _myVertexStage(this, shaderPass->vertexStage())
		, _myPixelStage(this, shaderPass->pixelStage())
	{
		_vertexStage	= &_myVertexStage;
		_pixelStage		= &_myPixelStage;
		

	}

	void Material_DX11::MyPass::onBind(RenderContext* _ctx, const VertexLayout* vertexLayout)
	{
		auto* ctx = static_cast<RenderContext_DX11*>(_ctx);
		_myVertexStage.bind(ctx, vertexLayout);
		_myPixelStage.bind(ctx, vertexLayout);
		onBindRenderState(ctx);

	}

	void Material_DX11::MyPass::onBindRenderState(RenderContext_DX11* ctx)
	{
		HRESULT hr;
		auto* renderer = ctx->renderer();
		auto dev = renderer->d3dDevice();
		const RenderState* _renderState = &(info()->renderState);

		if (!_rasterizerState)
		{
			ShaderCull cull;
			enumTryParse(cull, _renderState->cull);

			D3D11_RASTERIZER_DESC rasterDesc = {};
			rasterDesc.AntialiasedLineEnable = true;
			rasterDesc.CullMode = DX11Util::getDxCull(cull);
			rasterDesc.DepthBias = 0;
			rasterDesc.DepthBiasClamp = 0.0f;
			rasterDesc.DepthClipEnable = true;

			bool wireframe = true;
			rasterDesc.FillMode = wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;

			rasterDesc.FrontCounterClockwise = true;
			rasterDesc.MultisampleEnable = false;
			rasterDesc.ScissorEnable = false;
			rasterDesc.SlopeScaledDepthBias = 0.0f;

			hr = dev->CreateRasterizerState(&rasterDesc, _rasterizerState.ptrForInit());
			DX11Util::throwIfError(hr);
		}

		if (!_depthStencilState)
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

			DepthTest depthTest;
			enumTryParse(depthTest, _renderState->depthTest);

			if (_renderState->depthWrite == "On")
			{
				depthStencilDesc.DepthEnable = true;
				depthStencilDesc.DepthFunc = DX11Util::getDxDepthTest(depthTest);
			}
			else
			{
				depthStencilDesc.DepthEnable = false;
			}

			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.StencilEnable = false;
			depthStencilDesc.StencilReadMask = 0xFF;
			depthStencilDesc.StencilWriteMask = 0xFF;

			hr = dev->CreateDepthStencilState(&depthStencilDesc, _depthStencilState.ptrForInit());
			DX11Util::throwIfError(hr);
		}

		if (_blendState)
		{
			const RenderState::Blend* colorBlend;
			const RenderState::Blend* alphaBlend;

			colorBlend = &_renderState->blendRGB;
			alphaBlend = &_renderState->blendAlpha;

			D3D11_BLEND_DESC blendStateDesc = {};
			blendStateDesc.AlphaToCoverageEnable = false;
			blendStateDesc.IndependentBlendEnable = false;
			auto& rtDesc = blendStateDesc.RenderTarget[0];
			rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			BlendOP		blendOP;
			BlendFactor srcblend;
			BlendFactor destblend;

			enumTryParse(blendOP, colorBlend->op);
			enumTryParse(srcblend, colorBlend->src);
			enumTryParse(destblend, colorBlend->dest);

			rtDesc.BlendEnable = true;
			rtDesc.BlendOp		= DX11Util::getDxBlendOP(blendOP);
			rtDesc.SrcBlend		= DX11Util::getDxBlend(srcblend);
			rtDesc.DestBlend	= DX11Util::getDxBlend(destblend);

			enumTryParse(blendOP,	alphaBlend->op);
			enumTryParse(srcblend,	alphaBlend->src);
			enumTryParse(destblend, alphaBlend->dest);

			rtDesc.BlendOpAlpha		= DX11Util::getDxBlendOP(blendOP);
			rtDesc.SrcBlendAlpha	= DX11Util::getDxBlend(srcblend);
			rtDesc.DestBlendAlpha	= DX11Util::getDxBlend(destblend);

			hr = dev->CreateBlendState(&blendStateDesc, _blendState.ptrForInit());
			DX11Util::throwIfError(hr);
			
		}

		Color4f blendColor(1, 1, 1, 1);
		auto* dc = renderer->d3dDeviceContext();
		if (!_rasterizerState)	dc->RSSetState(_rasterizerState);
		if (!_depthStencilState) dc->OMSetDepthStencilState(_depthStencilState, 1);
		if (!_blendState)		dc->OMSetBlendState(_blendState, blendColor.data, 0xffffffff);
	}
}