#pragma once

#include "../Vertex/Vertex.h"
#include "Render_Common.h"
#include "sge_core.h"

namespace sge {

	class EditMesh : public NonCopyable
	{
	public:
		static const u8 kUvCountMax = 4;

		RenderPrimitiveType	primitive = RenderPrimitiveType::Triangles;
		Vector<u32> index;

		Vector<Tuple3f> pos;
		Vector<Tuple3f> O_pos;
		Vector<Color4b> color;

		Vector<Tuple2f>	uv[kUvCountMax];

		Vector<Tuple3f>	normal;
		Vector<Tuple3f>	tangent;
		Vector<Tuple3f>	binormal;

		void clear();
	private:

	};

}