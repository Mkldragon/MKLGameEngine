#pragma once
#include "sge_core.h"
#include "RenderRequest.h"
#include "RenderCommand.h"
#include "GameObject.h"
namespace sge
{
	class CRenderer;

	class RenderQueueObject : public RefCountBase
	{
	public:
		RenderQueueObject() = default;
		void Init(CRenderer* _rc);

		RenderMesh* GetRenderMesh() { return _mesh; }
		Material* GetMaterial() { return _mat; }
		CRenderer* _renderComponent;
	private:
		
		RenderMesh* _mesh = nullptr;
		Material* _mat = nullptr;
		RenderQueueObject(const RenderQueueObject&) = delete;
		void operator=(const RenderQueueObject&) = delete;
	};


	class RenderQueue : public NonCopyable
	{
	public:
		static RenderQueue* instance() { return s_instance; }
		RenderQueue();
		~RenderQueue();


		void DrawMeshes(RenderRequest* req);
		void RegisterRenderObject(RenderQueueObject* obj);

	private :
		static RenderQueue* s_instance;
		Vector<SPtr<RenderQueueObject>, 1024>  _renderObjs;
	};


}