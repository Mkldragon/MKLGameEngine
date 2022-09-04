
#include <sge_editor.h>
#include "Mesh/OBJ/ObjLoader.h"
#include "Mesh/OBJ/Terrain.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "DX11/Renderer_DX11.h"


namespace sge {

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	void onCreate(CreateDesc& desc) {

		SGE_DUMP_VAR(sizeof(Vertex_Pos));
		SGE_DUMP_VAR(sizeof(Vertex_PosColor));

		Base::onCreate(desc);
		auto* renderer = Renderer::instance();

		{
			RenderContext::CreateDesc renderContextDesc;
			renderContextDesc.window = this;
			_renderContext = renderer->createContext(renderContextDesc);
		}

		_camera.setPos(0, 5, 5);
		_camera.setAim(0, 0, 0);

		{
			Texture2D_CreateDesc texDesc;
			auto& image = texDesc.imageToUpload;
#if 0
			image.loadFile("Assets/Textures/TerrainTest.png");
			texDesc.size = image.size();
			texDesc.colorType = image.colorType();
#else
			int w = 256;
			int h = 256;
			texDesc.size.set(w, h);
			texDesc.colorType = ColorType::RGBAb;
			image.create(Color4b::kColorType, w, h);
			for (int y = 0; y < w; y++) 
			{
				auto span = image.row<Color4b>(y);
				for (int x = 0; x < h; x++) 
				{
					span[x] = Color4b(static_cast<u8>(x),
						static_cast<u8>(y),
						0,
						255);
				}
			}
			
#endif

			_testTexture = renderer->createTexture2D(texDesc);
			int a = 0;
	}

		

		auto shader = renderer->createShader("Assets/Shaders/Standard.shader");
		
		_material = renderer->createMaterial();
		_material->setShader(shader);

		//_material->setParam("mainTex", _testTexture);

		{
			EditMesh editMesh;
			ObjLoader::LoadFile(editMesh, "Assets/Mesh/test.obj");

			for (size_t i = editMesh.color.size(); i < editMesh.pos.size(); i++) {
				editMesh.color.emplace_back(255, 255, 255, 255);
			}
			_renderMesh.create(editMesh);
		}


		{
			EditMesh editMesh2;
			ObjLoader::LoadFile(editMesh2, "Assets/Mesh/Plane.obj");
			for (size_t i = editMesh2.color.size(); i < editMesh2.pos.size(); i++) {
				editMesh2.color.emplace_back(255, 255, 255, 255);
			}
			_renderMesh2.create(editMesh2);
		}

		{
			float size = 2048;
			float pos = size / -2;
			float y = -100;
			float height = 200;
			int maxLod = 6;
			_testTerrain.createFromHeightMapFile(
				Vec3f(pos, y, pos),
				Vec2f(size, size),
				height,
				maxLod,
				"Assets/Textures/TerrainTest.png");

		}
		//EditMesh _terrainEM;

		//int xSize = 1;
		//int zSize = 1;


		//_terrainEM.pos.emplace_back(-0.388876, -0.388876, 0);
		//_terrainEM.pos.emplace_back(0.388876, 0.388876, 0);
		//_terrainEM.pos.emplace_back(0.388876, -0.388876, 0);

		//_terrainEM.color.emplace_back(255, 255, 255, 255);
		//_terrainEM.color.emplace_back(255, 255, 255, 255);
		//_terrainEM.color.emplace_back(255, 255, 255, 255);

		//_terrainEM.uv[0].emplace_back(1, 0);
		//_terrainEM.uv[0].emplace_back(0, 1);
		//_terrainEM.uv[0].emplace_back(0, 0);

		//_terrainEM.normal.emplace_back(0.0f, 0.0f, 1.0f);
		//_terrainEM.normal.emplace_back(0.0f, 0.0f, 1.0f);
		//_terrainEM.normal.emplace_back(0.0f, 0.0f, 1.0f);
		//t = new Terrain(1, 1);
		// 
		//_testTerrain.CreateEditMesh(8, 8);
		//_terrain.create(*_testTerrain.getTerrainMesh());

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();

		auto* rd11 = Renderer_DX11::instance();

		ImGui_ImplWin32_Init(_hwnd);
		ImGui_ImplDX11_Init(rd11->d3dDevice(), rd11->d3dDeviceContext());

	}

	virtual void onCloseButton() override {
		NativeUIApp::current()->quit(0);

	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		if (ev.isDragging()) {
			using Button = UIMouseEventButton;
			switch (ev.pressedButtons) {
			case Button::Left: {
				//SGE_LOG("Left Clikc");
				auto d = ev.deltaPos * 0.01f;
				_camera.orbit(-d.x, -d.y);
			}break;

			case Button::Middle: {
				//SGE_LOG("Middle Click");
				auto d = ev.deltaPos * 0.005f;
				_camera.move(d.x, d.y, 0);
			}break;

			case Button::Right: {
				//SGE_LOG("Right Click");
				auto d = ev.deltaPos * -0.005f;
				_camera.dolly(d.x + d.y);
			}break;
			}
		}
	}

	virtual void onDraw() {
		Base::onDraw();
		if (!_renderContext) return;

		_camera.setViewport(clientRect());

		_renderContext->setFrameBufferSize(clientRect().size);
		_renderContext->beginRender();

		_renderRequest.reset();
		_renderRequest.matrix_model = Mat4f::s_identity();
		_renderRequest.matrix_view	= _camera.viewMatrix();
		_renderRequest.matrix_proj	= _camera.projMatrix();
		_renderRequest.camera_pos	= _camera.pos();

		_renderRequest.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });

		auto s = 1.0f;
		_material->setParam("test_float", s * 0.5f);
		_material->setParam("test_color", Color4f(s, s, s, 1));

		_renderRequest.drawMash(SGE_LOC, _renderMesh, _material);
		_testTerrain.render(_renderRequest, _material);


		_renderRequest.swapBuffers();

		_renderContext->commit(_renderRequest.commandBuffer);

		_renderContext->endRender();
		drawNeeded();

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Setting");
		ImGui::Button("TexT");
		ImGui::End();

		ImGui::Render();

	}

	SPtr<Material> _material;
	SPtr<Texture2D>	_testTexture;

	SPtr<RenderContext> _renderContext;
	RenderCommandBuffer _cmdBuf;
	RenderMesh	_renderMesh;
	RenderMesh	_renderMesh2;
	RenderMesh  _terrain;
	Terrain		_testTerrain;

	Math::Camera3f _camera;

	RenderRequest	_renderRequest;
	//Math	_camera;
};

class EditorApp : public NativeUIApp {
	using Base = NativeUIApp;
public:
	virtual void onCreate(CreateDesc& desc) override 
	{
		{
			String file = getExecutableFilename();
			String path = FilePath::getDir(file);
			path.append("/../../../../../../Data");
			SGE_LOG("dir = {}", path);
			auto* proj = ProjectSettings::instance();
			proj->setProjectRoot(path);

			auto dir = proj->importedPath();
		}
		Base::onCreate(desc);

		Renderer::CreateDesc renderDesc;
		renderDesc.apiType = Renderer::ApiType::DX11;
		Renderer::create(renderDesc);


		NativeUIWindow::CreateDesc winDesc;
		winDesc.isMainWindow = true;
		_mainWin.create(winDesc);
		_mainWin.setWindowTitle("SGE Editor");
	}



private:
	MainWin		_mainWin;
};

}

int main() {


	sge::EditorApp app;
	sge::EditorApp::CreateDesc desc;



	
	app.run(desc);

	return 0;
}
