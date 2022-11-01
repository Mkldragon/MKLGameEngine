
#include <sge_editor.h>
#include "Mesh/OBJ/ObjLoader.h"
#include "Mesh/OBJ/Terrain.h"
#include <sge_engine.h>
#include "GUI/HierarchyUI.h"
#include "GUI/InspectorUI.h"


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
			_material->setParam("mainTex", _testTexture);

			{
				
				ObjLoader::LoadFile(editMesh, "Assets/Mesh/test.obj");

				for (size_t i = editMesh.color.size(); i < editMesh.pos.size(); i++) {
					editMesh.color.emplace_back(255, 255, 255, 255);
				}
				_renderMesh.create(&editMesh);
			}


			{
				
				ObjLoader::LoadFile(editMesh2, "Assets/Mesh/Plane.obj");
				for (size_t i = editMesh2.color.size(); i < editMesh2.pos.size(); i++) {
					editMesh2.color.emplace_back(255, 255, 255, 255);
				}
				_renderMesh2.create(&editMesh2);
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


			{//Game Object Test
				gameObjManager.AddToList(&testObj);
				testObj.name = "TestObj1";
				testObj.AddComponent<BoxCollider>();

				gameObjManager.AddToList(&testObj2);

				gameObjManager.AddToList(&childrenObj);
				childrenObj.name = "Childen1";
				childrenObj.transform->setParent(testObj.transform);


				gameObjManager.AddToList(&childrenObj1);
				childrenObj1.name = "Childen2";
				childrenObj1.transform->setParent(testObj.transform);
				CRenderer* rc2 = childrenObj1.AddComponent<CRenderer>();
				rc2->SetUp(&_renderMesh, renderer->createMaterial());
				rc2->material->setShader(shader);
				rc2->material->setParam("mainTex", _testTexture);
				rc2->material->setParam("test_color", Color4f(1, 1, 1, 1));



				gameObjManager.AddToList(&childrenObj2); 
				childrenObj2.name = "Childen3";
				childrenObj2.transform->setParent(childrenObj.transform);
				CRenderer* rc = childrenObj2.AddComponent<CRenderer>();
				rc->SetUp(&_renderMesh2, renderer->createMaterial());
				rc->material->setShader(shader);
				rc->material->setParam("mainTex", _testTexture);
				rc->material->setParam("test_color", Color4f(1, 1, 1, 1));
			}



			_renderContext->RegisterGUILayer(&_inspector);
			_renderContext->RegisterGUILayer(&_hierarchy);
		}

		virtual void onCloseButton() override {
			NativeUIApp::current()->quit(0);

		}

		virtual void onUIMouseEvent(UIMouseEvent& ev) override {

			_renderContext->onUIMouseEvent(ev);

			if (ev.isCaptureImGui) return;

			if (ev.isDragging()) 
			{
				using Button = UIMouseEventButton;
				switch (ev.pressedButtons) {
				case Button::Left: {
					auto d = ev.deltaPos * 0.01f;
					_camera.orbit(-d.x, -d.y);
				}break;

				case Button::Middle: {
					auto d = ev.deltaPos * 0.005f;
					_camera.move(d.x, d.y, 0);
				}break;

				case Button::Right: {
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

			_renderContext->endRender();

			_renderRequest.reset();
			_renderRequest.matrix_view	= _camera.viewMatrix();
			_renderRequest.matrix_proj	= _camera.projMatrix();
			_renderRequest.camera_pos	= _camera.pos();

			_renderRequest.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });

			auto s = 1.0f;
			_material->setParam("test_color", Color4f(1, 1, 1, 1));
			//float t = cos(Counter += 0.01);
			//_material->setParam("test_float", s * 0.5f);
			//_testTerrain.render(_renderRequest);
			
			//_renderQueue->Render(&_renderRequest);
			//auto* rc = childrenObj2.GetComponent<CRenderer>()->renderMesh();
			_renderQueue.DrawMeshes(&_renderRequest);
			//_renderRequest.drawMash(SGE_LOC, _renderMesh, _material);
			//_renderRequest.drawMesh(SGE_LOC, *rc, _material);

			_renderRequest.swapBuffers();
			_renderContext->commit(_renderRequest.commandBuffer);
			_renderContext->endRender();

			drawNeeded();
		}

		SPtr<Material> _material;
		SPtr<Texture2D>	_testTexture;

		HierarchyUI _hierarchy;
		InspectorUI _inspector;


		GameObject	testObj;
		GameObject	testObj2;
		GameObject	childrenObj;
		GameObject	childrenObj1;
		GameObject	childrenObj2;

		//EditMesh

		//Renderer_DX11* renderer_dx11;
		bool show_demo_window = true;
		float Counter = 0;
		SPtr<RenderContext> _renderContext;
		RenderCommandBuffer _cmdBuf;

		RenderMesh	_renderMesh;
		RenderMesh	_renderMesh2;
		EditMesh	editMesh2;
		EditMesh	editMesh;

		RenderMesh  _terrain;
		GameObjectManager gameObjManager;
		Terrain		_testTerrain;
		

		Math::Camera3f _camera;

		
		RenderRequest	_renderRequest;
		RenderQueue		_renderQueue;

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



	void TestReflection(Object& obj)
	{
		std::cout << "-------------------------------------\n";

		auto* t = typeOf(obj);
		if (!t)
		{
			std::cout << "unknown Type\n";
			return;
		}

		std::cout << t->name << "\n";
		for (auto& f : t->fields())
		{
			std::cout << f.name << " Type=" << f.fieldType->name << " offset:" << f.offset << "\n";
		}

	}


}



int main() {


	sge::EditorApp app;
	sge::EditorApp::CreateDesc desc;
	
	app.run(desc);

	return 0;
}
