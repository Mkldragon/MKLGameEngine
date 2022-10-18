#include "EditMesh.h"

namespace sge
{
	void EditMesh::clear()
	{
		index.clear();
		pos.clear();
		color.clear();
		normal.clear();
		tangent.clear();
		binormal.clear();

		for (int i = 0; i < kUvCountMax; i++)
		{
			uv[i].clear();
		}

	}


}