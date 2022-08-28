#include "RenderContext_DX11.h"
#include "Renderer_DX11.h"
#include "RenderGpuBuffer_DX11.h"
#include "Material_DX11.h"

namespace sge
{

	RenderContext_DX11::RenderContext_DX11(CreateDesc & desc) : RenderContext(desc)
	{
		_renderer = Renderer_DX11::instance();

		auto* win = static_cast<NativeUIWindow_Win32*>(desc.window);

		ID3D11Device1* dev			= _renderer->d3dDevice();
		IDXGIFactory1* dxgiFactory	= _renderer->dxgiFactory();


		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
			swapChainDesc.BufferDesc.Width = 8;
			swapChainDesc.BufferDesc.Height = 8;
			swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferCount = 1;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.OutputWindow = win->_hwnd;
			swapChainDesc.Windowed = TRUE;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swapChainDesc.Flags = 0;

			auto hr = dxgiFactory->CreateSwapChain(dev, &swapChainDesc, _swapChain.ptrForInit());
			DX11Util::throwIfError(hr);
		}
	}

	void RenderContext_DX11::onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd)
	{

		auto* ctx = _renderer->d3dDeviceContext();
		if (_renderTargetView && cmd.color.has_value()) {
			float color[] = { 0.0f, 0.2f, 0.4f, 1.0f };
			ctx->ClearRenderTargetView(_renderTargetView, cmd.color->data);
		}
		if (_depthStencilView && cmd.depth.has_value()) {
			ctx->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, *cmd.depth, 0);
		}

	}

	void RenderContext_DX11::onCmd_SwapBuffers(RenderCommand_SwapBuffers& cmd)
	{
		_swapChain->Present(_renderer->vsync() ? 1 : 0, 0);
	}

	void RenderContext_DX11::onCmd_DrawCall(RenderCommand_DrawCall& cmd)
	{

		if (!cmd.vertexLayout) { SGE_ASSERT(false); return; }
		auto* vertexBuffer = static_cast<RenderGpuBuffer_DX11*>(cmd.vertexBuffer.ptr());
		if (!vertexBuffer) { SGE_ASSERT(false); return; }

		if (cmd.vertexCount <= 0) { SGE_ASSERT(false); return; }
		if (cmd.primitive == RenderPrimitiveType::None) { SGE_ASSERT(false); return; }


		RenderGpuBuffer_DX11* indexBuffer = nullptr;
		if (cmd.indexCount > 0) {
			indexBuffer = static_cast<RenderGpuBuffer_DX11*>(cmd.indexBuffer.ptr());
			if (!indexBuffer) { SGE_ASSERT(false); return; }
		}


		auto* ctx = _renderer->d3dDeviceContext();

		if (auto* pass = cmd.getMaterialPass()) {
			pass->bind(this, cmd.vertexLayout);
		}
		else {
			_setTestShaders(cmd.vertexLayout);
		}
		//_setTestShaders();

		
		auto primitive = DX11Util::getDxPrimitiveTopology(cmd.primitive);
		ctx->IASetPrimitiveTopology(primitive);

		/*auto* inputLayout = _getTestInputLayout(cmd.vertexLayout);
		if (!inputLayout) { SGE_ASSERT(false); return; }

		ctx->IASetInputLayout(inputLayout);*/

		UINT stride = static_cast<UINT>(cmd.vertexLayout->stride);
		UINT offset = 0;
		UINT vertexCount = static_cast<UINT>(cmd.vertexCount);
		UINT indexCount = static_cast<UINT>(cmd.indexCount);

		ID3D11Buffer* ppVertexBuffers[] = { vertexBuffer->d3dBuf() };
		ctx->IASetVertexBuffers(0, 1, ppVertexBuffers, &stride, &offset);
		

		if (indexCount > 0) {
			auto indexType = DX11Util::getDxFormat(cmd.indexType);
			ctx->IASetIndexBuffer(indexBuffer->d3dBuf(), indexType, 0);
			ctx->DrawIndexed(indexCount, 0, 0);
		}
		else {
			ctx->Draw(vertexCount, 0);
		}
	}

	ID3D11InputLayout* RenderContext_DX11::_getTestInputLayout(const VertexLayout* src)
	{
		if (!src) return nullptr;

		auto it = _testInputLayouts.find(src);
		if (it != _testInputLayouts.end()) {
			return it->second;
		}

		Vector<D3D11_INPUT_ELEMENT_DESC, 32> inputDesc;



		for (auto& e : src->elements) {
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

		ComPtr<ID3D11InputLayout>	outLayout;
		

		auto* dev = _renderer->d3dDevice();

		auto hr = dev->CreateInputLayout(inputDesc.data(), static_cast<UINT>(inputDesc.size()), _testVertexShaderBytecode->GetBufferPointer(),
								_testVertexShaderBytecode->GetBufferSize(), outLayout.ptrForInit());

		DX11Util::throwIfError(hr);

		_testInputLayouts[src] = outLayout;
		return outLayout;
	}

	void RenderContext_DX11::onSetFrameBufferSize(Vec2f newSize)
	{
		_renderTargetView.reset(nullptr); // release buffer and render target view before resize
		_depthStencilView.reset(nullptr);

		_swapChain->ResizeBuffers(0
			, static_cast<UINT>(Math::max(0.0f, newSize.x))
			, static_cast<UINT>(Math::max(0.0f, newSize.y))
			, DXGI_FORMAT_UNKNOWN
			, 0);

	}

	void RenderContext_DX11::onBeginRender()
	{

		ID3D11DeviceContext4* ctx = _renderer->d3dDeviceContext();

		if (!_renderTargetView) {
			_createRenderTarget();
		}

		ID3D11RenderTargetView* rt[] = { _renderTargetView };
		ctx->OMSetRenderTargets(1, rt, _depthStencilView);

		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = _frameBufferSize.x;
		viewport.Height = _frameBufferSize.y;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;

		ctx->RSSetViewports(1, &viewport);
	}

	void RenderContext_DX11::onEndRender()
	{
	}

	

	void RenderContext_DX11::_createRenderTarget()
	{
		auto* renderer = Renderer_DX11::instance();
		auto* dev = renderer->d3dDevice();

		HRESULT hr;

		ComPtr<ID3D11Texture2D> backBuffer;
		hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.ptrForInit()));
		DX11Util::throwIfError(hr);

		hr = dev->CreateRenderTargetView(backBuffer, nullptr, _renderTargetView.ptrForInit());
		DX11Util::throwIfError(hr);
		

		D3D11_TEXTURE2D_DESC backBufferDesc;
		backBuffer->GetDesc(&backBufferDesc);

		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = backBufferDesc.Width;
		descDepth.Height = backBufferDesc.Height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = dev->CreateTexture2D(&descDepth, nullptr, _depthStencil.ptrForInit());
		DX11Util::throwIfError(hr);

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		hr = dev->CreateDepthStencilView(_depthStencil, &descDSV, _depthStencilView.ptrForInit());
		DX11Util::throwIfError(hr);
		//backBuffer->Release();
	}

	void RenderContext_DX11::_setTestShaders(const VertexLayout* vertexLayout)
	{
		ID3D11Device1*			dev = _renderer->d3dDevice();
		ID3D11DeviceContext4*	ctx = _renderer->d3dDeviceContext();

		HRESULT hr;

		if (!_testVertexShader)
		{
			ComPtr<ID3D10Blob> VS;
			D3DCompileFromFile(L"Assets/Shader/Triangle.hlsl", 0, 0, "vs_main", "vs_5_0", 0, 0, VS.ptrForInit(), 0);
			
			_testVertexShaderBytecode = VS;
			
			dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), nullptr, _testVertexShader.ptrForInit());

		}

		if (!_testPixelShader)
		{
			ComPtr<ID3D10Blob> PS;
			D3DCompileFromFile(L"Assets/Shader/Triangle.hlsl", 0, 0, "ps_main", "ps_5_0", 0, 0, PS.ptrForInit(), 0);
			dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), nullptr, _testPixelShader.ptrForInit());
		}




		if (!_testRasterizerState) {
			D3D11_RASTERIZER_DESC rasterDesc = {};
			rasterDesc.AntialiasedLineEnable = true;
			rasterDesc.CullMode = D3D11_CULL_NONE;
			rasterDesc.DepthBias = 0;
			rasterDesc.DepthBiasClamp = 0.0f;
			rasterDesc.DepthClipEnable = true;

			bool wireframe = true;
			rasterDesc.FillMode = wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;

			rasterDesc.FrontCounterClockwise = true;
			rasterDesc.MultisampleEnable = false;
			rasterDesc.ScissorEnable = false;
			rasterDesc.SlopeScaledDepthBias = 0.0f;

			hr = dev->CreateRasterizerState(&rasterDesc, _testRasterizerState.ptrForInit());
			DX11Util::throwIfError(hr);

		}

		if (!_testDepthStencilState) {
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

			bool depthTest = true;
			if (depthTest) {
				depthStencilDesc.DepthEnable = true;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			}
			else {
				depthStencilDesc.DepthEnable = false;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			}

			//depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.StencilEnable = false;
			depthStencilDesc.StencilReadMask = 0xFF;
			depthStencilDesc.StencilWriteMask = 0xFF;

			hr = dev->CreateDepthStencilState(&depthStencilDesc, _testDepthStencilState.ptrForInit());
			DX11Util::throwIfError(hr);
		}

		if (!_testBlendState) {
			D3D11_BLEND_DESC blendStateDesc = {};
			blendStateDesc.AlphaToCoverageEnable = false;
			blendStateDesc.IndependentBlendEnable = false;
			auto& rtDesc = blendStateDesc.RenderTarget[0];

			rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			bool blendEnable = true;
			if (blendEnable) {
				rtDesc.BlendEnable = true;

				rtDesc.BlendOp			= D3D11_BLEND_OP_ADD;
				rtDesc.SrcBlend			= D3D11_BLEND_SRC_ALPHA;
				rtDesc.DestBlend		= D3D11_BLEND_INV_SRC_ALPHA;

				rtDesc.BlendOpAlpha		= D3D11_BLEND_OP_ADD;
				rtDesc.SrcBlendAlpha	= D3D11_BLEND_SRC_ALPHA;
				rtDesc.DestBlendAlpha	= D3D11_BLEND_INV_SRC_ALPHA;
			}
			else {
				rtDesc.BlendEnable = false;
			}

			hr = dev->CreateBlendState(&blendStateDesc, _testBlendState.ptrForInit());
			DX11Util::throwIfError(hr);
		}

		ctx->RSSetState(_testRasterizerState);
		ctx->OMSetDepthStencilState(_testDepthStencilState, 1);

		Color4f blendColor(1, 1, 1, 1);
		ctx->OMSetBlendState(_testBlendState, blendColor.data, 0xffffffff);

		ctx->VSSetShader(_testVertexShader, 0, 0);
		ctx->PSSetShader(_testPixelShader, 0, 0);

		auto* inputLayout = _getTestInputLayout(vertexLayout);
		if (!inputLayout) { SGE_ASSERT(false); return; }
		ctx->IASetInputLayout(inputLayout);
	}


	void RenderContext_DX11::onCommit(RenderCommandBuffer& cmdBuf)
	{
		_dispatch(this, cmdBuf);
	}

}