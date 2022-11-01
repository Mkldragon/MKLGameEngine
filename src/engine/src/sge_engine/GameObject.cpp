#include "GameObject.h"
#include "sge_core/ECS/TypeReflection.h"

namespace sge
{
	const TypeInfo* Collider::s_getType()
	{
		class TI : public TI_Base
		{
		public:
			TI()
			{
				static FieldInfo fi[] =
				{
					FieldInfo("isTrigger", &This::isTrigger),
				};
				setField(fi);
			}
		};

		static TI ti;
		return &ti;
	}

	const TypeInfo* BoxCollider::s_getType()
	{
		class TI : public TI_Base
		{
		public:
			TI()
			{
				static FieldInfo fi[] =
				{
					FieldInfo("Center", &This::center),
					FieldInfo("Size", &This::size),
				};
				setField(fi);
			}
		};

		static TI ti;
		return &ti;
	}


	const TypeInfo* GameObject::s_getType()
	{
		class TI : public TI_Base
		{
		public:
			TI()
			{
				static FieldInfo fi[] =
				{
					FieldInfo("Objtest1", &This::Objtest1),
					FieldInfo("Objtest2", &This::Objtest2),
				};
				setField(fi);
			}
		};

		static TI ti;
		return &ti;
	}


	const TypeInfo* Component::s_getType()
	{
		class TI : public TI_Base
		{
		public:
			TI()
			{
				static FieldInfo fi[] =
				{
					FieldInfo("instanceID", &This::instanceID),
				};
				setField(fi);
			}
		};

		static TI ti;
		return &ti;
	}

	const TypeInfo* Transform::s_getType()
	{
		class TI : public TI_Base
		{
		public:
			TI()
			{
				static FieldInfo fi[] =
				{
					FieldInfo("position", &This::position),
					FieldInfo("rotation", &This::rotation),
					FieldInfo("localScale", &This::localScale),
				};
				setField(fi);
			}
		};

		static TI ti;
		return &ti;
	}

	void Transform::_computeLocalMatrix() {
		_localMatrix = Mat4f::s_TRS(_localPos, _localRotate, _localScale);
	}

	void Transform::_computeWorldMatrix() {
		if (parent) {
			_worldMatrix = parent->worldMatrix() * localMatrix();
		}
		else {
			_worldMatrix = localMatrix();
		}
	}

	void Transform::_setWorldMatrixDirty() {
		if (_dirty.worldMatrix) return;
		_dirty.worldMatrix = true;
		for (auto* c : _children) {
			if (!c) continue;
			c->_setWorldMatrixDirty();
		}
	}


	const TypeInfo* CRenderer::s_getType()
	{
		class TI : public TI_Base
		{
		public:
			TI()
			{
				static FieldInfo fi[] =
				{
					FieldInfo("componet1", &This::isCastShadow),
				};
				setField(fi);
			}
		};

		static TI ti;
		return &ti;
	}

	void CRenderer::QueueRegister()
	{
		queueObj = new RenderQueueObject();
		queueObj->Init(this);
		RenderQueue* r = RenderQueue::instance();
		r->RegisterRenderObject(queueObj);

		int a = 0;
	}

	void CRenderer::SetUp(RenderMesh* rendermesh, Material* mat)
	{
		_rendermesh = rendermesh;
		material = mat;
	}

	const TypeInfo* Rigidbody::s_getType()
	{
		class TI : public TI_Base
		{
		public:
			TI()
			{
				static FieldInfo fi[] =
				{
					FieldInfo("childtest1", &This::child2test1),
					FieldInfo("childtest2", &This::child2test2),
					FieldInfo("test4", &This::test4),
				};
				setField(fi);
			}
		};

		static TI ti;
		return &ti;
	}




	GameObjectManager* GameObjectManager::s_instance = nullptr;
	GameObjectManager::GameObjectManager()
	{
		SGE_ASSERT(s_instance == nullptr);
		s_instance = this;
	}

	GameObjectManager::~GameObjectManager()
	{
		SGE_ASSERT(s_instance == this);
		s_instance = nullptr;
	}

	void GameObjectManager::AddToList(Object* obj)
	{
		gameObjectData.emplace_back(obj);
	}



	void GameObjectManager::SelectHirearchyObject(GameObject* obj)
	{
		selectedObject = obj;
	}

}