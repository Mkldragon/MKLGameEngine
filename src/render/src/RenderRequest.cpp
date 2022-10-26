#include "RenderRequest.h"

namespace sge
{
	RenderRequest::RenderRequest()
	{
		light_pos = Vec3f(10, 10, 0);
		light_dir = Vec3f(-5, -10, -2);
		light_power = 4.0f;
		light_color = Vec3f(1, 1, 1);
	}
	void RenderRequest::reset()
	{
		commandBuffer.reset();
	}
	void RenderRequest::setMaterialCommonParams(Material* mat)
	{
		if (!mat) return;

		//mat->setParam("sge_matrix_model", matrix_model);
		mat->setParam("sge_matrix_view", matrix_view);
		mat->setParam("sge_matrix_proj", matrix_proj);

		auto mvp = matrix_proj * matrix_view * matrix_model;
		mat->setParam("sge_matrix_mvp", mvp);

		mat->setParam("sge_camera_pos", camera_pos);

		mat->setParam("sge_light_pos", light_pos);
		mat->setParam("sge_light_dir", light_dir);
		mat->setParam("sge_light_power", light_power);
		mat->setParam("sge_light_color", light_color);

	}
	void RenderRequest::drawMesh(const SrcLoc& debugLoc, const RenderMesh& mesh, Material* material)
	{
		for (auto& sm : mesh.subMeshes()) {
			drawSubMesh(debugLoc, sm, material);
		}

	}
	void RenderRequest::drawSubMesh(const SrcLoc& debugLoc, const RenderSubMesh& subMesh, Material* mat)
	{
		if (!mat) { SGE_ASSERT(false); return; }

		setMaterialCommonParams(mat);

		auto passes = mat->passes();

		for (size_t i = 0; i < passes.size(); i++) {
			auto* cmd = commandBuffer.newCommand<RenderCommand_DrawCall>();
#if _DEBUG
			//cmd->debugLoc = debugLoc;
#endif

			cmd->material = mat;
			cmd->materialPassIndex = i;

			cmd->primitive = subMesh.primitive();
			cmd->vertexLayout = subMesh.vertexLayout();
			cmd->vertexBuffer = subMesh.vertexBuffer();
			cmd->vertexCount = subMesh.vertexCount();
			cmd->indexBuffer = subMesh.indexBuffer();
			cmd->indexType = subMesh.indexType();
			cmd->indexCount = subMesh.indexCount();
		}

	}
}
