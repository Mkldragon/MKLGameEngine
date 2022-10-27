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

			if (obj->_renderComponent == nullptr) return;
			if (obj->_renderComponent->_rendermesh == nullptr ||
				obj->_renderComponent->material == nullptr) 
				return;


			req->matrix_model = obj->_renderComponent->getTransMatrix();


			req->drawMesh(SGE_LOC, *obj->_renderComponent->_rendermesh, obj->_renderComponent->material);
		}


	}


	void RenderQueue::RegisterRenderObject(RenderQueueObject* obj)
	{
		int a = 0;
		_renderObjs.emplace_back(obj);
	}
	void RenderQueueObject::Init(CRenderer* _rc)
	{
		_renderComponent = _rc;
		_mesh = _renderComponent->_rendermesh;
		_mat = _renderComponent->material;
	}
	//RenderQueueObject::RenderQueueObject(RenderMesh* mesh, Material* mat)
	//{
	//	_mesh = mesh;
	//	_mat = mat;
	//}
}