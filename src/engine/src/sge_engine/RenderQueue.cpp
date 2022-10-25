#include "RenderQueue.h"

namespace sge
{


	RenderQueue* RenderQueue::s_instance = nullptr;
	RenderQueue::RenderQueue()
	{
		SGE_ASSERT(s_instance == nullptr);
		s_instance = this;
	}
	RenderQueue::~RenderQueue()
	{
		SGE_ASSERT(s_instance == nullptr);
		s_instance = this;
	}

	void RenderQueue::DrawMeshes(RenderRequest* req)
	{
		if (_renderObjs.size() == 0) return;

		for (size_t i = 0; i < _renderObjs.size(); i++)
		{
			RenderQueueObject* obj = _renderObjs[i].ptr();
			if (obj->GetMaterial() == nullptr || obj->GetMaterial() == nullptr) return;

			req->drawMesh(SGE_LOC,  *obj->GetRenderMesh(), obj->GetMaterial());
		}
	}


	void RenderQueue::RegisterRenderObject(RenderQueueObject* obj)
	{
		int a = 0;
		_renderObjs.emplace_back(obj);
	}
	void RenderQueueObject::Init(RenderMesh* mesh, Material* mat)
	{
		_mesh = mesh;
		_mat = mat;
	}
	//RenderQueueObject::RenderQueueObject(RenderMesh* mesh, Material* mat)
	//{
	//	_mesh = mesh;
	//	_mat = mat;
	//}
}