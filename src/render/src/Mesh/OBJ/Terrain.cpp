#include "Terrain.h"
#include "Renderer.h"

namespace sge
{

	void Terrain::createFromHeightMapFile(const Vec3f& terrainPos, const Vec2f& terrainSize, float terrainHeight, int maxLod, StrView heightMapFilename) 
	{
		Image img;
		img.loadPngFile(heightMapFilename);
		int a = 0;
		CreateEditMesh(terrainPos, terrainSize, terrainHeight, maxLod, img);
	}

	void Terrain::CreateEditMesh(const Vec3f terrainPos, const Vec2f& terrainSize, float terrainHeight, int maxLod, const Image& heightMap)
	{
		destory();

		if (maxLod < 1)
			maxLod = 1;

		if (maxLod > 8)
			throw SGE_ERROR("Reach max lod Limit");
		_heightMapResolution.set(heightMap.width(), heightMap.height());
		_terrainPos = terrainPos;
		_terrainSize = terrainSize;
		_terrainHeight = terrainHeight;

		_maxLod = maxLod;

		_patchCount = (_heightMapResolution - 1) / patchCellsPerRow();

		_patchLevels.resize(maxLod);

		for (int lv = 0; lv < _patchLevels.size(); lv++) {
			_patchLevels[lv].create(this, lv);
		}

		{ // Patches
			_patches.resize(_patchCount.x * _patchCount.y);
			auto shader = Renderer::instance()->createShader("Assets/Shaders/terrain.shader");

			auto* p = _patches.begin();
			for (int y = 0; y < _patchCount.y; y++) {
				for (int x = 0; x < _patchCount.x; x++) {
					//Vec2i patchPos{ x, y };
					p->create(this, Vec2i(x, y), shader);
					p++;
				}
			}
			SGE_ASSERT(p == _patches.end());
		}

		_vertexLayout = Vertex_Pos2f::layout();
		auto* renderer = Renderer::instance();

		{
			Vector<Vertex_Pos2f> vertexData;
			int verticesPerRow = patchVerticesPerRow();
			int cellsPerRow = patchCellsPerRow();

			_vertexCount = verticesPerRow * verticesPerRow;
			vertexData.resize(_vertexCount);

			auto scale = patchSize();

			auto* dst = vertexData.begin();
			for (int y = 0; y < verticesPerRow; y++)
			{
				for (int x = 0; x < verticesPerRow; x++)
				{
					//Vec2f _pos{ static_cast<float>(x), static_cast<float>(y) };
					dst->pos = Vec2f::s_cast(Vec2i(x, y)) * scale / static_cast<float>(cellsPerRow);
					dst++;
				}
			}

			SGE_ASSERT(dst == vertexData.end());

			RenderGpuBuffer::CreateDesc desc;
			desc.type = RenderGpuBufferType::Vertex;
			desc.bufferSize = _vertexCount * _vertexLayout->stride;
			_vertexBuffer = renderer->createGpuBuffer(desc);
			_vertexBuffer->uploadToGpu(spanCast<u8>(vertexData.span()));
		}

		{
			Texture2D_CreateDesc desc;
			desc.size = heightMap.size();
			desc.colorType = heightMap.colorType();
			desc.imageToUpload.copy(heightMap);
			desc.samplerState.filter = TextureFilter::Point;
			desc.samplerState.wrapU = TextureWrap::Clamp;
			desc.samplerState.wrapV = TextureWrap::Clamp;

			_heightMapTexture = renderer->createTexture2D(desc);
		}

	}

	void Terrain::destory()
	{
		_terrainSize.set(0, 0);
		_terrainHeight = 0;
		_heightMapResolution.set(0, 0);
		_maxLod = 1;
		_patchCount.set(0, 0);
		_patchLevels.clear();

		_vertexBuffer = nullptr;
		_vertexLayout = nullptr;
		_vertexCount = 0;

	}

	void Terrain::render(RenderRequest& req, Material* mat)
	{
		for (auto& p : _patches) {
			p.setDisplayLevelByViewPos(req.camera_pos);
		}

		for (auto& p : _patches) {
			p.render(req, mat);
		}

	}


	void Terrain::Patch::render(RenderRequest& req, Material* mat)
	{
		auto zoneMask = ZoneMask::None;
		if (_adjacentPatchHasHigherLod(0, -1)) zoneMask |= ZoneMask::North;
		if (_adjacentPatchHasHigherLod(1, 0))  zoneMask |= ZoneMask::East;
		if (_adjacentPatchHasHigherLod(0, 1))  zoneMask |= ZoneMask::South;
		if (_adjacentPatchHasHigherLod(-1, 0)) zoneMask |= ZoneMask::West;

		auto lv = Math::clamp(_displayLevel, int(0), _terrain->maxLod() - 1);

		auto* pi = _terrain->patchIndices(lv, zoneMask);
		if (!pi) { SGE_ASSERT(false); return; }


		if (!_material) { SGE_ASSERT(false); return; }

		req.setMaterialCommonParams(_material);

		_material->setParam("terrainHeightMap", _terrain->heightMapTexture());

		_material->setParam("terrainPos", _terrain->terrainPos());
		_material->setParam("terrainSize", _terrain->terrainSize());
		_material->setParam("terrainHeight", _terrain->terrainHeight());

		_material->setParam("patchCellsPerRow", _terrain->patchCellsPerRow());

		_material->setParam("patchIndex", _index);
		_material->setParam("patchSize", _terrain->patchSize());

		auto passes = _material->passes();

		for (size_t i = 0; i < passes.size(); i++) {
			auto* cmd = req.commandBuffer.newCommand<RenderCommand_DrawCall>();
#if _DEBUG
			//cmd->debugLoc = SGE_LOC;
#endif

			cmd->material = _material;
			cmd->materialPassIndex = i;

			cmd->primitive = RenderPrimitiveType::Triangles;
			cmd->vertexLayout = _terrain->vertexLayout();
			cmd->vertexBuffer = _terrain->vertexBuffer();
			cmd->vertexCount = _terrain->vertexCount();
			cmd->indexBuffer = pi->indexBuffer();;
			cmd->indexType = pi->indexType();
			cmd->indexCount = pi->indexCount();
		}
	}



	void Terrain::Patch::create(Terrain* terrain, const Vec2i& index, Shader* shader)
	{
		_terrain = terrain;
		_index = index;

		_material = Renderer::instance()->createMaterial();
		_material->setShader(shader);

	}

	void Terrain::Patch::setDisplayLevelByViewPos(const Vec3f& viewPos) {
		auto distance = (worldCenterPos() - viewPos).length();
		auto d = _terrain->patchSize().x * 5;
		_displayLevel = static_cast<int>(distance / d);
	}

	void Terrain::PatchIndices::create(Terrain* terrain, int level)
	{
		_terrain = terrain;
		_level = level;

		_Indices.resize(s_patchMeshCount);
		auto zoneMask = ZoneMask::None;
		for (auto& it : _Indices) {
			it.create(terrain, _level, zoneMask);
			zoneMask += 1;
		}

	}

	void Terrain::Indices::create(Terrain* terrain, int level, ZoneMask zoneMask)
	{
		Vector<u16> indexData;

		int verticesPerRow = terrain->patchVerticesPerRow();
		int rows = 1 << (terrain->maxLod() - 1 - level);
		int n = rows / 2;
		int step = 1 << level;

		if (rows == 1) {
			u16 x0 = 0;
			u16 x1 = static_cast<u16>(verticesPerRow - 1);
			u16 y0 = 0;
			u16 y1 = static_cast<u16>((verticesPerRow - 1) * verticesPerRow);

			indexData.resize(6);

			indexData[0] = x0 + y0;
			indexData[1] = x1 + y1;
			indexData[2] = x1 + y0;

			indexData[3] = x0 + y0;
			indexData[4] = x0 + y1;
			indexData[5] = x1 + y1;

		}

		else 
		{
			Vector<Vec2i, 256> sector0; // lod 0
			Vector<Vec2i, 256> sector1; // lod 1

			for (int y = 0; y < n; y++) {
				int lastRow = (y == n - 1) ? 1 : 0;

				for (int x = 0; x <= y; x++) {
					Vec2i v[3];
					int odd = (x + y) % 2;

					v[0] = Vec2i(x, y) * step;
					v[1] = Vec2i(x + 1, y + 1 - odd) * step;
					v[2] = Vec2i(x, y + 1) * step;
					sector0.appendRange(v);

					if (lastRow) {
						v[2].x = (x - 1 + odd) * step;
					}
					sector1.appendRange(v);

					if (x == y) break; // drop last triangle in this row

					v[0] = Vec2i(x, y + odd) * step;
					v[1] = Vec2i(x + 1, y) * step;
					v[2] = Vec2i(x + 1, y + 1) * step;

					sector0.appendRange(v);

					if (!lastRow || !odd) { // drop even number triangle in last row
						sector1.appendRange(v);
					}
				}
			}

			{ // north
				auto& sector = enumHas(zoneMask, ZoneMask::North) ? sector1 : sector0;
				_addToIndices(indexData, sector, verticesPerRow, Vec2i(1, -1), false);
				_addToIndices(indexData, sector, verticesPerRow, Vec2i(-1, -1), false);
			}
			{ // east
				auto& sector = enumHas(zoneMask, ZoneMask::East) ? sector1 : sector0;
				_addToIndices(indexData, sector, verticesPerRow, Vec2i(1, 1), true);
				_addToIndices(indexData, sector, verticesPerRow, Vec2i(-1, 1), true);
			}
			{ // south
				auto& sector = enumHas(zoneMask, ZoneMask::South) ? sector1 : sector0;
				_addToIndices(indexData, sector, verticesPerRow, Vec2i(1, 1), false);
				_addToIndices(indexData, sector, verticesPerRow, Vec2i(-1, 1), false);
			}
			{ // west
				auto& sector = enumHas(zoneMask, ZoneMask::West) ? sector1 : sector0;
				_addToIndices(indexData, sector, verticesPerRow, Vec2i(1, -1), true);
				_addToIndices(indexData, sector, verticesPerRow, Vec2i(-1, -1), true);
			}
		}


		{
			auto* renderer = Renderer::instance();

			auto byteSpan = spanCast<const u8>(indexData.span());

			RenderGpuBuffer::CreateDesc desc;
			desc.type = RenderGpuBufferType::Index;
			desc.bufferSize = byteSpan.size();

			_indexCount = indexData.size();
			_indexBuffer = renderer->createGpuBuffer(desc);
			_indexBuffer->uploadToGpu(byteSpan);

		}

	}

	void Terrain::Indices::_addToIndices(Vector<u16>& vertexIndex, Span<Vec2i> sector, int verticesPerRow, Vec2i direction, bool flip)
	{
		auto w = verticesPerRow;
		Vec2i center(w / 2, w / 2);
		int verticesPerPatch = verticesPerRow * verticesPerRow;


		Span<u16>	dstSpan;
		{
			auto oldSize = vertexIndex.size();
			auto newSize = oldSize + sector.size();
			vertexIndex.resize(newSize);
			dstSpan = vertexIndex.subspan(oldSize);
		}

		{
			auto* dst = dstSpan.begin();
			for (auto s : sector)
			{
				s = s * direction + center;
				if (flip) s = s.yx();
				SGE_ASSERT(s.x >= 0 && s.y >= 0);

				auto vi = static_cast<u16>(s.x + s.y * verticesPerRow);
				SGE_ASSERT(vi < verticesPerPatch);

				*dst = vi;
				dst++;
			}
			SGE_ASSERT(dst == dstSpan.end());
		}

		int rx = direction.x < 0 ? 1 : 0;
		int ry = direction.y < 0 ? 1 : 0;
		int rf = flip ? 0 : 1;

		if (rx ^ ry ^ rf) {
			auto* dst = dstSpan.begin();
			auto* end = dstSpan.end();
			for (; dst < end; dst += 3) {
				swap(dst[0], dst[1]);
			}
		}

	}


	inline
		bool Terrain::Patch::_adjacentPatchHasHigherLod(int x, int y) const
	{
		auto* adj = _terrain->patch(_index.x + x, _index.y + y);
		if (!adj) return false;
		return adj->displayLevel() > _displayLevel;
	}

}

#pragma region self
//void Terrain::CreateEditMesh(int _length, int _width)
//{
//	xSize = _width;
//	zSize = _length;
//	totalSize = xSize * zSize;
//	int GridXSize = xSize * patchsize;
//	int GridZSize = zSize * patchsize;

//	for (int z = 0; z <= GridZSize; z++)
//	{
//		for (int x = 0; x <= GridXSize; x++)
//		{
//			float xPos = 0.25f * x;
//			float zPos = 0.25f * z;
//			_vertex.emplace_back(xPos, zPos, 0);
//		}
//	}

//	_patches.resize(totalSize);
//	auto* p = _patches.begin();

//	for (int z = 0; z < zSize; z++)
//	{
//		for (int x = 0; x < xSize; x++)
//		{
//			p->create(this, Vec2i(x, z));
//			p++;

//		}
//	}

//	p = _patches.begin();
//	for (int z = 0; z < zSize; z++)
//	{
//		for (int x = 0; x < xSize; x++)
//		{
//			p->splitTriangle();
//			p++;

//		}
//	}
//}

//bool Terrain::comparePatchLOD(int curPatchIndex, int dir)
//{
//	auto *p		= _patches.begin(); 
//	auto* dir_p = _patches.begin();
//	p += curPatchIndex;
//	int y = 0;
//	int x = 0;

//	switch (dir)
//	{
//	case 0:
//		y = curPatchIndex - xSize;
//		if (y >= 0)
//		{
//			dir_p = p - xSize;
//			if (p->LOD > dir_p->LOD) return true;;
//		}
//		break;
//	case 1:
//		x = curPatchIndex % xSize + 1;
//		if (x < xSize)
//		{
//			dir_p = p + 1;
//			if (p->LOD > dir_p->LOD) return true;
//		}
//		break;
//	case 2:
//		y = curPatchIndex + xSize;
//		if (y < totalSize)
//		{
//			dir_p = p + xSize;
//			if (p->LOD > dir_p->LOD) return true;
//		}
//		break;
//	case 3:
//		x = curPatchIndex % xSize - 1;
//		if (x >= 0)
//		{
//			dir_p = p - 1;
//			if (p->LOD > dir_p->LOD) return true;
//		}
//		break;
//	default:
//		SGE_LOG("Dir Out of Range");
//		break;

//	}

//	return false;
//}
//void Terrain::emplaceVertex(const Vector<int> &vertexIndex)
//{
//	auto* v = vertexIndex.begin();
//	while (v != vertexIndex.end())
//	{
//		_terrainMesh.pos.emplace_back(_vertex[*v]);
//		_terrainMesh.color.emplace_back(255, 255, 255, 255);
//		_terrainMesh.uv[0].emplace_back(0, 0);
//		_terrainMesh.normal.emplace_back(0.0f, 0.0f, 1.0f);
//		v++;
//	}
//	
//}

//void Terrain::Patch::create(Terrain* terrain, const Vec2i& pos)
//{
//	_terrain = terrain;
//	_pos = pos;
//	float testLOD = _pos.x + _pos.y;

//	if (testLOD < 4)
//		LOD = 0;
//	else if (testLOD < 8)
//		LOD = 1;
//	else
//		LOD = 2;

//}

//void Terrain::Patch::splitTriangle()
//{
//	auto* t = _triangles.begin();
//	int patchIndex = _pos.x + _pos.y * _terrain->xSize;
//	
//	for (int i = 0; i < 4; i++)
//	{
//		if (_terrain->comparePatchLOD(patchIndex, i))
//		{
//			t->create(_terrain, this, i, true);
//			SGE_LOG("Patch Index : {}", patchIndex);
//		}
//		else
//		{
//			t->create(_terrain, this, i, false);
//		}
//		t++;
//	}
//}

//void Terrain::Patch::subdivision(int dir)
//{
//	auto* t = _triangles.begin();
//	t += dir;
//	t->subdivision();
//}



//void Terrain::GridTriangle::create(Terrain* terrain, Patch* patch, int index, bool split)
//{
//	_patch			= patch;
//	_patchLOD		= _patch->LOD;
//	Vec2i pos		= _patch->getPatchPos();
//	_terrain		= terrain;
//	int patchSize	= _terrain->patchsize;
//	int terrainSize = _terrain->xSize * patchSize;

//	int x = patchSize * pos.x + (terrainSize * 4 + 4) * pos.y;

//	int v0 = x;
//	int v1 = x + 4;
//	int v2 = x + terrainSize * 4 + 8;
//	int v3 = x + terrainSize * 4 + 4;

//	int center = x + terrainSize * 2 + 4;

//	switch (index)
//	{
//	case 0:
//		if (_patchLOD == 2)
//		{
//			if (split)
//			{
//				subdivision(v0, center, v1);
//			}
//			else
//			{
//				triangleIndex.emplace_back(v0);
//				triangleIndex.emplace_back(center);
//				triangleIndex.emplace_back(v1);
//			}

//			//_subTriangle.emplace_back(Vec3i{ v0 ,center, v1 });
//		}
//		else
//			subdivision(v0, center, v1, _patchLOD, split);
//		break;

//	case 1:		
//		if (_patchLOD == 2)
//		{
//			if (split)
//			{
//				subdivision(v1, center, v2);
//			}
//			else
//			{
//				triangleIndex.emplace_back(v1);
//				triangleIndex.emplace_back(center);
//				triangleIndex.emplace_back(v2);
//			}
//			//_subTriangle.emplace_back(Vec3i{ v1 ,center, v2 });
//		}
//		else
//			subdivision(v1, center, v2, _patchLOD, split);
//	case 2:
//		if (_patchLOD == 2)
//		{
//			if (split)
//			{
//				subdivision(v2, center, v3);
//			}
//			else
//			{
//				triangleIndex.emplace_back(v2);
//				triangleIndex.emplace_back(center);
//				triangleIndex.emplace_back(v3);
//			}
//		}
//		else
//			subdivision(v2, center, v3, _patchLOD, split);
//		break;
//	case 3:
//		if (_patchLOD == 2)
//		{
//			if (split)
//			{
//				subdivision(v3, center, v0);
//			}
//			else
//			{
//				triangleIndex.emplace_back(v3);
//				triangleIndex.emplace_back(center);
//				triangleIndex.emplace_back(v0);
//			}
//		}
//		else
//			subdivision(v3, center, v0, _patchLOD, split);
//		break;
//	default:
//		break;
//	}

//	terrain->emplaceVertex(triangleIndex);

//}
//void Terrain::GridTriangle::subdivision(int _v0, int _v1, int _v2, int _sdCount, bool split)
//{

//	int remainLOD = _terrain->MaxLOD - _sdCount;

//	if (remainLOD == 0)
//	{

//		triangleIndex.emplace_back(_v0);
//		triangleIndex.emplace_back(_v1);
//		triangleIndex.emplace_back(_v2);
//		
//		return;
//	}
//	else
//	{
//		int v0 = _v0;
//		int v1 = _v1;
//		int v2 = _v2;
//		int center = (_v0 + _v2) / 2;
//		subdivision(v1, center, v0, _sdCount + 1, 0, split);
//		subdivision(v2, center, v1, _sdCount + 1, 1, split);

//	}

//}
//void Terrain::GridTriangle::subdivision(int _v0, int _v1, int _v2, int _sdCount, int LOR, bool split)
//{
//	int remainLOD = _terrain->MaxLOD - _sdCount;

//	if (remainLOD == 0)
//	{
//		if (LOR == 1 && split)
//		{
//			subdivision(_v0, _v1, _v2);
//		}
//		else
//		{
//			triangleIndex.emplace_back(_v0);
//			triangleIndex.emplace_back(_v1);
//			triangleIndex.emplace_back(_v2);
//		}

//		return;
//	}
//	else
//	{
//		int v0 = _v0;
//		int v1 = _v1;
//		int v2 = _v2;
//		int center = (_v0 + _v2) / 2;
//		subdivision(v1, center, v0, _sdCount + 1, LOR == 0 ? 0 : 1, split);
//		subdivision(v2, center, v1, _sdCount + 1, LOR == 1 ? 0 : 1, split);

//	}
//}
//void Terrain::GridTriangle::subdivision(int _v0, int _v1, int _v2)
//{

//	int v0 = _v0;
//	int v1 = _v1;
//	int v2 = _v2;
//	int center = (_v0 + _v2) / 2;


//	triangleIndex.emplace_back(v1);
//	triangleIndex.emplace_back(center);
//	triangleIndex.emplace_back(v0);

//	triangleIndex.emplace_back(v2);
//	triangleIndex.emplace_back(center);
//	triangleIndex.emplace_back(v1);

//	_terrain->emplaceVertex(triangleIndex);
//}

//void Terrain::GridTriangle::subdivision()
//{
//	auto v = _subTriangle.begin();
//	for (int i = 0; i < _subTriangle.size(); i++)
//	{
//		subdivision(v->x, v->y, v->z);
//		v++;
//	}
//}


#pragma endregion




	