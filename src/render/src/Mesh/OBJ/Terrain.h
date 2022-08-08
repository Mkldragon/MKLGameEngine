#pragma once

#include "Mesh/EditMesh.h"

namespace sge
{
#define PatchDirection_ENUM_LIST(E) \
	E(None, = 0) \
	E(North, = 1 << 0)  \
	E(East,  = 1 << 1)  \
	E(South, = 1 << 2)  \
	E(West,  = 1 << 3)  \
//----
SGE_ENUM_CLASS(PatchDirection, u8);
SGE_ENUM_ALL_OPERATOR(PatchDirection)


	class Terrain : public NonCopyable
	{
	public:
		int			xSize = 0;
		int			zSize = 0;
		int			totalSize = 0;

		const int	patchsize = 4;
		const int	MaxLOD = 3;

		void CreateEditMesh(int length, int width);
		EditMesh* getTerrainMesh() { return &_terrainMesh; }
		bool comparePatchLOD(int curPatchIndex, int dir);

		class Patch;

		class GridTriangle
		{
		public:
			void create(Terrain* terrain, Patch* patch, int index, bool split = false);
			void subdivision(int _v0, int _center, int _v1, int _lodremain, bool split);
			void subdivision(int _v0, int _center, int _v1, int _lodremain, int LOR, bool split);
			void subdivision(int _v0, int _center, int _v1);
			void subdivision();
		private:
			int				_patchLOD;
			int				_lod;
			Patch*			_patch = nullptr;;
			Vector<int>		triangleIndex;
			Vector<Vec3i>	_subTriangle;
			Terrain*		_terrain = nullptr;
		};

		class Patch
		{
		public:
			int LOD = 0;
			void	create(Terrain* terrain, const Vec2i& pos);
			void	splitTriangle();
			void	subdivision(int dir);
			const	Vector_<GridTriangle, 4>& triangles() const { return _triangles; }
			int		getPatchIndex() { return _pos.x * _pos.y; }
			const	Vec2i	getPatchPos() const { return _pos; }
		private:
			Terrain*					_terrain = nullptr;
			Vec2i						_pos{ 0, 0 };
			Vector_<GridTriangle, 4>	_triangles;
		};

	const	Vector<Vec3f>&	terrainVertex() const { return _vertex; }
			void			emplaceVertex(const Vector<int> &vertexIndex);

	private:

		EditMesh		_terrainMesh;
		Vec3f			_cameraPos{ 0,0,0 };
		Vector<Vec3f>	_vertex;
		Vector<Patch>	_patches;
	};


}