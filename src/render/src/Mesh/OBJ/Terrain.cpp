#include "Terrain.h"

namespace sge
{

	void Terrain::CreateEditMesh(int _length, int _width)
	{
		xSize = _width;
		zSize = _length;
		totalSize = xSize * zSize;
		int GridXSize = xSize * patchsize;
		int GridZSize = zSize * patchsize;

		for (int z = 0; z <= GridZSize; z++)
		{
			for (int x = 0; x <= GridXSize; x++)
			{
				float xPos = 0.25f * x;
				float zPos = 0.25f * z;
				_vertex.emplace_back(xPos, zPos, 0);
			}
		}

		_patches.resize(totalSize);
		auto* p = _patches.begin();

		for (int z = 0; z < zSize; z++)
		{
			for (int x = 0; x < xSize; x++)
			{
				p->create(this, Vec2i(x, z));
				p++;

			}
		}

		p = _patches.begin();
		for (int z = 0; z < zSize; z++)
		{
			for (int x = 0; x < xSize; x++)
			{
				p->splitTriangle();
				p++;

			}
		}
	}

	bool Terrain::comparePatchLOD(int curPatchIndex, int dir)
	{
		auto *p		= _patches.begin(); 
		auto* dir_p = _patches.begin();
		p += curPatchIndex;
		int y = 0;
		int x = 0;

		switch (dir)
		{
		case 0:
			y = curPatchIndex - xSize;
			if (y >= 0)
			{
				dir_p = p - xSize;
				if (p->LOD > dir_p->LOD) return true;;
			}
			break;
		case 1:
			x = curPatchIndex % xSize + 1;
			if (x < xSize)
			{
				dir_p = p + 1;
				if (p->LOD > dir_p->LOD) return true;
			}
			break;
		case 2:
			y = curPatchIndex + xSize;
			if (y < totalSize)
			{
				dir_p = p + xSize;
				if (p->LOD > dir_p->LOD) return true;
			}
			break;
		case 3:
			x = curPatchIndex % xSize - 1;
			if (x >= 0)
			{
				dir_p = p - 1;
				if (p->LOD > dir_p->LOD) return true;
			}
			break;
		default:
			SGE_LOG("Dir Out of Range");
			break;

		}

		return false;
	}
	void Terrain::emplaceVertex(const Vector<int> &vertexIndex)
	{
		auto* v = vertexIndex.begin();
		while (v != vertexIndex.end())
		{
			_terrainMesh.pos.emplace_back(_vertex[*v]);
			_terrainMesh.color.emplace_back(255, 255, 255, 255);
			_terrainMesh.uv[0].emplace_back(0, 0);
			_terrainMesh.normal.emplace_back(0.0f, 0.0f, 1.0f);
			v++;
		}
		
	}

	void Terrain::Patch::create(Terrain* terrain, const Vec2i& pos)
	{
		_terrain = terrain;
		_pos = pos;
		float testLOD = _pos.x + _pos.y;

		if (testLOD < 4)
			LOD = 0;
		else if (testLOD < 8)
			LOD = 1;
		else
			LOD = 2;

	}

	void Terrain::Patch::splitTriangle()
	{
		auto* t = _triangles.begin();
		int patchIndex = _pos.x + _pos.y * _terrain->xSize;
		
		for (int i = 0; i < 4; i++)
		{
			if (_terrain->comparePatchLOD(patchIndex, i))
			{
				t->create(_terrain, this, i, true);
				SGE_LOG("Patch Index : {}", patchIndex);
			}
			else
			{
				t->create(_terrain, this, i, false);
			}
			t++;
		}
	}

	void Terrain::Patch::subdivision(int dir)
	{
		auto* t = _triangles.begin();
		t += dir;
		t->subdivision();
	}



	void Terrain::GridTriangle::create(Terrain* terrain, Patch* patch, int index, bool split)
	{
		_patch			= patch;
		_patchLOD		= _patch->LOD;
		Vec2i pos		= _patch->getPatchPos();
		_terrain		= terrain;
		int patchSize	= _terrain->patchsize;
		int terrainSize = _terrain->xSize * patchSize;

		int x = patchSize * pos.x + (terrainSize * 4 + 4) * pos.y;

		int v0 = x;
		int v1 = x + 4;
		int v2 = x + terrainSize * 4 + 8;
		int v3 = x + terrainSize * 4 + 4;

		int center = x + terrainSize * 2 + 4;

		switch (index)
		{
		case 0:
			if (_patchLOD == 2)
			{
				if (split)
				{
					subdivision(v0, center, v1);
				}
				else
				{
					triangleIndex.emplace_back(v0);
					triangleIndex.emplace_back(center);
					triangleIndex.emplace_back(v1);
				}

				//_subTriangle.emplace_back(Vec3i{ v0 ,center, v1 });
			}
			else
				subdivision(v0, center, v1, _patchLOD, split);
			break;

		case 1:		
			if (_patchLOD == 2)
			{
				if (split)
				{
					subdivision(v1, center, v2);
				}
				else
				{
					triangleIndex.emplace_back(v1);
					triangleIndex.emplace_back(center);
					triangleIndex.emplace_back(v2);
				}
				//_subTriangle.emplace_back(Vec3i{ v1 ,center, v2 });
			}
			else
				subdivision(v1, center, v2, _patchLOD, split);
		case 2:
			if (_patchLOD == 2)
			{
				if (split)
				{
					subdivision(v2, center, v3);
				}
				else
				{
					triangleIndex.emplace_back(v2);
					triangleIndex.emplace_back(center);
					triangleIndex.emplace_back(v3);
				}
			}
			else
				subdivision(v2, center, v3, _patchLOD, split);
			break;
		case 3:
			if (_patchLOD == 2)
			{
				if (split)
				{
					subdivision(v3, center, v0);
				}
				else
				{
					triangleIndex.emplace_back(v3);
					triangleIndex.emplace_back(center);
					triangleIndex.emplace_back(v0);
				}
			}
			else
				subdivision(v3, center, v0, _patchLOD, split);
			break;
		default:
			break;
		}

		terrain->emplaceVertex(triangleIndex);

	}
	void Terrain::GridTriangle::subdivision(int _v0, int _v1, int _v2, int _sdCount, bool split)
	{

		int remainLOD = _terrain->MaxLOD - _sdCount;

		if (remainLOD == 0)
		{

			triangleIndex.emplace_back(_v0);
			triangleIndex.emplace_back(_v1);
			triangleIndex.emplace_back(_v2);
			
			return;
		}
		else
		{
			int v0 = _v0;
			int v1 = _v1;
			int v2 = _v2;
			int center = (_v0 + _v2) / 2;
			subdivision(v1, center, v0, _sdCount + 1, 0, split);
			subdivision(v2, center, v1, _sdCount + 1, 1, split);

		}

	}
	void Terrain::GridTriangle::subdivision(int _v0, int _v1, int _v2, int _sdCount, int LOR, bool split)
	{
		int remainLOD = _terrain->MaxLOD - _sdCount;

		if (remainLOD == 0)
		{
			if (LOR == 1 && split)
			{
				subdivision(_v0, _v1, _v2);
			}
			else
			{
				triangleIndex.emplace_back(_v0);
				triangleIndex.emplace_back(_v1);
				triangleIndex.emplace_back(_v2);
			}

			return;
		}
		else
		{
			int v0 = _v0;
			int v1 = _v1;
			int v2 = _v2;
			int center = (_v0 + _v2) / 2;
			subdivision(v1, center, v0, _sdCount + 1, LOR == 0 ? 0 : 1, split);
			subdivision(v2, center, v1, _sdCount + 1, LOR == 1 ? 0 : 1, split);

		}
	}
	void Terrain::GridTriangle::subdivision(int _v0, int _v1, int _v2)
	{

		int v0 = _v0;
		int v1 = _v1;
		int v2 = _v2;
		int center = (_v0 + _v2) / 2;


		triangleIndex.emplace_back(v1);
		triangleIndex.emplace_back(center);
		triangleIndex.emplace_back(v0);

		triangleIndex.emplace_back(v2);
		triangleIndex.emplace_back(center);
		triangleIndex.emplace_back(v1);

		_terrain->emplaceVertex(triangleIndex);
	}

	void Terrain::GridTriangle::subdivision()
	{
		auto v = _subTriangle.begin();
		for (int i = 0; i < _subTriangle.size(); i++)
		{
			subdivision(v->x, v->y, v->z);
			v++;
		}
	}
}
