
#pragma once
#include "sge_core.h"
#include "SgeImGui.h"
#include "RenderQueue.h"

namespace sge
{

	class RenderQueueObject;
	class GameObject;
	class Component;
	class Transform;


	class Component : public Object
	{
		SGE_TYPE(Component, Object);
	public:
		Component() = default;
		int instanceID;
		GameObject* gameObject;

	};

	class Transform : public Component
	{
		SGE_TYPE(Transform, Component);
	public:

		Transform* parent;


		Vec3f position{ 0, 0, 0 };
		Vec3f rotation{ 0, 0, 0 };
		Vec3f localScale{ 1, 1, 1 };


		void setLocalPos(float x, float y, float z) { setLocalPos(Vec3f(x, y, z)); }
		void setLocalPos(const Vec3f& v)			{ _localPos = v;		_setLocalMatrixDirty(); }
		void setLocalRotate(const Quat4f& v)		{ _localRotate = v;		_setLocalMatrixDirty(); }
		void setLocalScale(const Vec3f& v)			{ _localScale = v;		_setLocalMatrixDirty(); }


		const Vec3f& localPos()		const { return _localPos; }
		const Quat4f& localRotate() const { return _localRotate; }
		const Vec3f& localScale()	const { return _localScale; }


		const Mat4f PosMatrix() {
			return Mat4f::s_translate(position);
		}

		void setParent(Transform* p)
		{
			parent = p;
			p->AddChildren(this);
		};

		Transform* getChild(int index)
		{
			if (index >= _children.size()) return  nullptr;
			else return _children[index];
		};

		void AddChildren(Transform* child)
		{
			_children.emplace_back(child);
		};
		size_t getChildCount() const { return _children.size(); };

		const Mat4f& localMatrix();
		const Mat4f& worldMatrix();


	private:
		void _computeLocalMatrix();
		void _computeWorldMatrix();

		void _setLocalMatrixDirty();
		void _setWorldMatrixDirty();

		Vec3f	_localPos	{ 0,0,0 };
		Quat4f	_localRotate{ 0,0,0,0 };
		Vec3f	_localScale	{ 1,1,1 };


		Vector<Transform*, 64> _children;

		struct Dirty {
			Dirty()
				: localMatrix(true)
				, worldMatrix(true)
			{}
			bool	localMatrix : 1;
			bool	worldMatrix : 1;
		};

		Dirty		_dirty;
		Mat4f	_localMatrix;
		Mat4f	_worldMatrix;

	};
	inline
		void Transform::_setLocalMatrixDirty() {
		if (_dirty.localMatrix) return;
		_dirty.localMatrix = true;
		_setWorldMatrixDirty();
	}

	inline
		const Mat4f& Transform::localMatrix() {
		if (_dirty.localMatrix) {
			_dirty.localMatrix = false;
			_computeLocalMatrix();
		}
		return _localMatrix;
	}

	inline
		const Mat4f& Transform::worldMatrix() {
		if (_dirty.worldMatrix) {
			_dirty.worldMatrix = false;
			_computeWorldMatrix();
		}
		return _worldMatrix;
	}



	class GameObject : public Object
	{
		SGE_TYPE(GameObject, Object);
	public:
		GameObject() { transform = this->AddComponent<Transform>(); }
		String		name = "GameObject";
		float		Objtest1 = 0;
		int			Objtest2 = 0;

		void AddComponentToObj(Component* component)
		{
			component->gameObject = this;
			this->_component.emplace_back(component);
		}

		template<class T>
		T* GetComponent()
		{
			for (auto* c = _component.begin(); c != _component.end(); c++)
			{
				if (c->ptr()->getType()->isKindOf<T>())
					return my_cast<T>(c->ptr());
			}
			return nullptr;
		}


		template<class T>
		T* AddComponent()
		{
			T* t = new T();
			this->AddComponentToObj(t);
			return t;
		}

		Transform* transform;
		Vector<SPtr<Component>, 32> _component;


	private:
	};


	class Collider : public Component
	{
		SGE_TYPE(Collider, Component);
	public:
		bool isTrigger = false;

	};

	class BoxCollider : public Collider
	{
		SGE_TYPE(BoxCollider, Collider);
	public:
		Vec3f center{ 0, 0, 0 };
		Vec3f size{ 0,0,0 };

	};



	class CRenderer : public Component
	{
		SGE_TYPE(CRenderer, Component);
		
	public:

		CRenderer() { QueueRegister(); }
		~CRenderer() { }

		void QueueRegister();

		void SetUp(RenderMesh* rendermesh, Material* mat);


		Mat4f getTransMatrix()
		{
			Mat4f objTransMat = Mat4f::s_identity();
			Transform* trans = Base::gameObject->transform;

			objTransMat = objTransMat * Mat4f::s_rotate(trans->rotation);
			objTransMat = objTransMat * Mat4f::s_translate(trans->position);
			objTransMat = objTransMat * Mat4f::s_scale(trans->localScale);

			return objTransMat;
		}
		RenderMesh* renderMesh()
		{
			GameObject* obj = Base::gameObject;
			Transform* trans = obj->transform;

			if (material != nullptr && obj != nullptr)
			{
				Mat4f posMatrix = trans->PosMatrix();
				material->setParam("sge_matrix_model", posMatrix);

			}
			return _rendermesh;
		}

		RenderQueueObject* queueObj;
		bool isCastShadow = false;
		RenderMesh* _rendermesh = nullptr;
		SPtr<Material> material = nullptr;

	};

	class Rigidbody : public Component
	{
		SGE_TYPE(Rigidbody, Component);
	public:

		Vec3f	child2test1{ 0,1,1 };
		int		child2test2 = 0;
		float	test4 = 10;
	};








	class GameObjectManager : public NonCopyable
	{
	public:
		static GameObjectManager* instance() { return s_instance; }
		GameObjectManager();
		~GameObjectManager();

		void CreateGameObject();
		void AddToList(Object* obj);
		void SelectHirearchyObject(GameObject* obj);
		Vector<SPtr<Object> , 1024> gameObjectData;
		GameObject* selectedObject = nullptr;
	private:
		static GameObjectManager* s_instance;
		Component component;
		Transform childComponent;
	};


}