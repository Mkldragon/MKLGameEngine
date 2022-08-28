
#include <sge_editor.h>
#include "Mesh/OBJ/ObjLoader.h"
#include "Mesh/OBJ/Terrain.h"

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
			image.loadFile("Assets/Textures/uvChecker.png");
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
		{

			auto model = Mat4f::s_identity();
			auto view = _camera.viewMatrix();
			auto proj = _camera.projMatrix();
			auto mvp = proj * view * model;

			_material->setParam("sge_matrix_model", model);
			_material->setParam("sge_matrix_view", view);
			_material->setParam("sge_matrix_proj", proj);
			_material->setParam("sge_matrix_mvp", mvp);

			_material->setParam("sge_camera_pos", _camera.pos());

			_material->setParam("sge_light_pos", Vec3f(10, 10, 0));
			_material->setParam("sge_light_dir", Vec3f(-5, -10, -2));
			_material->setParam("sge_light_power", 4.0f);
			_material->setParam("sge_light_color", Vec3f(1, 1, 1));
		}


		auto s = 1.0f;
		_material->setParam("test_float", s * 0.5f);
		_material->setParam("test_color", Color4f(s, s, s, 1));

		_renderContext->setFrameBufferSize(clientRect().size);
		_renderContext->beginRender();

		_cmdBuf.reset();
		_cmdBuf.clearFrameBuffers()->setColor({ 0.0f , 0, 0.2f, 1 });
		//_cmdBuf.drawMesh(SGE_LOC, _renderMesh, _material);
		//_cmdBuf.drawMesh(SGE_LOC, _renderMesh2, _material);
		_cmdBuf.drawMesh(SGE_LOC, _terrain, _material);

		_cmdBuf.swapBuffers();

		_renderContext->commit(_cmdBuf);

		_renderContext->endRender();
		drawNeeded();
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
