#pragma once

#include "Mesh/EditMesh.h"
#include "RenderRequest.h"

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
		using ZoneMask = PatchDirection;

		void CreateEditMesh(const Vec3f terrainPos, const Vec2f& terrainSize, int length, int width, int maxLod);
		void destory();
		void render(RenderRequest& req, Material* mat);


		class Indices
		{
		public:
			void create(Terrain* terrain, int level, ZoneMask zoneMask);
			RenderGpuBuffer* indexBuffer() const { return constCast(_indexBuffer.ptr()); }
			size_t indexCount() const { return _indexCount; }
			RenderDataType indexType() const { return RenderDataTypeUtil::get<u8>(); }
			
		private:
			void _addToIndices(Vector<u16>& vertexIndex, Span<Vec2i> sector, int verticesPerRow, Vec2i direction, bool flip);

			SPtr<RenderGpuBuffer> _indexBuffer;
			size_t _indexCount = 0;

		};

		class PatchIndices
		{
		public:
			static const int s_patchMeshCount = 16;
			void create(Terrain* terrain, int level);
			Indices* indices(ZoneMask zoneMask) { return &_Indices[enumInt(zoneMask)]; }
		private:
			Terrain* _terrain = nullptr;
			int _level = 0;
			Vector<Indices, s_patchMeshCount> _Indices;
		};

		class Patch
		{
		public:
			void create(Terrain* terrain, const Vec2i& index, Shader* shader);
			int displayLevel() const { return _displayLevel; }

			void render(RenderRequest& req, Material* mat);

			Vec3f worldCenterPos();
			void setDisplayLevelByViewPos(const Vec3f& viewPos);

		private:

			inline	bool _adjacentPatchHasHigherLod(int x, int y) const;

			int					_displayLevel = 0;
			Vec2i				_index{ 0,0 };
			Terrain*			_terrain = nullptr;
			SPtr<Material>		_material;
			
		};

		//class GridTriangle
		//{
		//public:
		//	void create(Terrain* terrain, Patch* patch, int index, bool split = false);
		//	void subdivision(int _v0, int _center, int _v1, int _lodremain, bool split);
		//	void subdivision(int _v0, int _center, int _v1, int _lodremain, int LOR, bool split);
		//	void subdivision(int _v0, int _center, int _v1);
		//	void subdivision();
		//private:
		//	int				_patchLOD;
		//	int				_lod;
		//	Patch*			_patch = nullptr;;
		//	Vector<int>		triangleIndex;
		//	Vector<Vec3i>	_subTriangle;
		//	Terrain*		_terrain = nullptr;
		//};

		//class Patch
		//{
		//public:
		//	int LOD = 0;
		//	void	create(Terrain* terrain, const Vec2i& pos);
		//	void	splitTriangle();
		//	void	subdivision(int dir);
		//	const	Vector_<GridTriangle, 4>& triangles() const { return _triangles; }
		//	int		getPatchIndex() { return _pos.x * _pos.y; }
		//	const	Vec2i	getPatchPos() const { return _pos; }
		//private:
		//	Terrain*					_terrain = nullptr;
		//	Vec2i						_pos{ 0, 0 };
		//	Vector_<GridTriangle, 4>	_triangles;
		//};

	//const	Vector<Vec3f>&	terrainVertex() const { return _vertex; }
			//void			emplaceVertex(const Vector<int> &vertexIndex);

		int patchCellsPerRow() const { return 1 << (_maxLod - 1); }
		int patchVerticesPerRow() const { return patchCellsPerRow() + 1; }
		int	maxLod() const { return _maxLod; }

		const	Vec3f& terrainPos() const { return _terrainPos; }
		const	Vec2f& terrainSize() const { return _terrainSize; }

		Vec2i	patchCount() const { return _patchCount; }

		Vec2f	patchSize() const { return  {_terrainSize.x/_patchCount.x, _terrainSize.y/_patchCount.y }; }

		Vec2f	cellSize() const { return { _terrainSize.x / (_heightMapResolution.x - 1), 
											_terrainSize.y / (_heightMapResolution.y - 1)}; }

		Indices* patchIndices(int level, ZoneMask zoneMask);
		Patch* patch(int x, int y);

	private:
		//EditMesh		_terrainMesh;
		//Vec3f			_cameraPos{ 0,0,0 };
		//Vector<Vec3f>	_vertex;
		Vec3f _terrainPos{ 0,0,0 };
		Vec2f _terrainSize{ 0,0 };
		float _terrainHeight = 0;
		Vec2i _heightMapResolution{ 0,0 };
		int _maxLod = 1;
		Vec2i _patchCount{ 0,0 };

		Vector<Patch>	_patches;
		Vector<PatchIndices> _patchLevels;

		const VertexLayout* _vertexLayout = nullptr;
		size_t _vertexCount = 0;
		SPtr<RenderGpuBuffer> _vertexBuffer;
		
	};

	SGE_INLINE Terrain::Indices* Terrain::patchIndices(int level, ZoneMask zoneMask) {
		if (level < 0 || level >= _patchLevels.size()) {
			SGE_ASSERT(false);
			return nullptr;
		}

		auto& lv = _patchLevels[level];
		return lv.indices(zoneMask);
	}


	SGE_INLINE Terrain::Patch* Terrain::patch(int x, int y) {
		if (x < 0 || y < 0 || x >= _patchCount.x || y >= _patchCount.y)
			return nullptr;
		return &_patches[y * _patchCount.x + x];
	}



	SGE_INLINE Vec3f Terrain::Patch::worldCenterPos() {
		auto s = _terrain->patchSize();
		Vec2f pos {static_cast<float>(_index.x) , static_cast<float>(_index.y) };
		pos = (pos + 0.5) * s;
		auto o = _terrain->terrainPos() + Vec3f(pos.x, 0, pos.y);
		return o;
	}
}