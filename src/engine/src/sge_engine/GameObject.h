
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
		int getChildCount() { return _children.size(); };

	private:
		Vector<Transform*, 64> _children;
		bool isDirty;


	};

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
			objTransMat = Mat4f::s_scale(trans->localScale);
			objTransMat = objTransMat * Mat4f::s_rotate(trans->rotation);
			objTransMat = objTransMat * Mat4f::s_translate(trans->position);

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
		Material* material = nullptr;

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