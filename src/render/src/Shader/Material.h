#pragma once

#include "Shader.h"
#include <RenderGpuBuffer.h>
#include <texture/Texture.h>


namespace sge
{
	class RenderContext;
	class Material;
	class MaterialPass;


	struct MaterialPass_Stage : public NonCopyable
	{
		//using M_Pass = MaterialPass;
	public:
		virtual ~MaterialPass_Stage() = default;
		MaterialPass_Stage(MaterialPass* pass, ShaderStage* shaderStage);

		const ShaderDescData* Info() const { return _shaderStage->info(); }

	friend class MaterialPass;
	protected:
		struct ConstBuffer
		{
		public:
			using DataType = RenderDataType;
			using Info = ShaderDescData::ConstBufferInfo;
			using VarInfo = ShaderDescData::ShaderVariable;

			Vector<u8>				cpuBuffer;
			SPtr<RenderGpuBuffer>	gpuBuffer;

			template<class V>
			void setParam(const Info& cbInfo, StrView name, const V& v)
			{
				
				auto* varInfo = cbInfo.findVariable(name);
				if (!varInfo) return;
				_setParam(varInfo, v);
			}

			void create(const Info& info);
			void uploadToGpu();

			const Info* info() const { return _info; }

		private:
			const Info* _info		= nullptr;
			bool		_gpuDirty	= false;

			void _setParam(const VarInfo* varInfo, const i32&		value)	{ _setParamCheckType(varInfo, value); }
			void _setParam(const VarInfo* varInfo, const u32&		value)	{ _setParamCheckType(varInfo, value); }
			void _setParam(const VarInfo* varInfo, const float&		value)	{ _setParamCheckType(varInfo, value); }
			void _setParam(const VarInfo* varInfo, const Tuple2i&	value)	{ _setParamCheckType(varInfo, value); }
			void _setParam(const VarInfo* varInfo, const Tuple3i&	value)	{ _setParamCheckType(varInfo, value); }
			void _setParam(const VarInfo* varInfo, const Tuple4i&	value)	{ _setParamCheckType(varInfo, value); }
			void _setParam(const VarInfo* varInfo, const Tuple2f&	value)	{ _setParamCheckType(varInfo, value); }
			void _setParam(const VarInfo* varInfo, const Tuple3f&	value)	{ _setParamCheckType(varInfo, value); }
			void _setParam(const VarInfo* varInfo, const Tuple4f&	value)	{ _setParamCheckType(varInfo, value); }
			void _setParam(const VarInfo* varInfo, const Mat4f&		value)	{ _setParamCheckType(varInfo, value); }

			template<class V>
			void _setParamCheckType(const VarInfo* varInfo, const V& value) {
				if (varInfo->dataType != RenderDataTypeUtil::get<V>()) {
					errorType();
					return;
				}
				_setValueAs(varInfo, value);
			}

			template<class V>
			void _setValueAs(const VarInfo* varInfo, const V& value) 
			{
				auto end = varInfo->offset + sizeof(value);
				if (end > cpuBuffer.size())
					throw SGE_ERROR("ConstBuffer setParam out of range");
				auto* dst = cpuBuffer.data() + varInfo->offset;
				*reinterpret_cast<V*>(dst) = value;
				_gpuDirty = true;
			}

			void errorType();
		};

		template<class V>
		void _setParam(StrView name, const V& v)
		{
			if (_shaderStage == nullptr) return;
			size_t i = 0;
			for (auto& cb : _constBufs)
			{
				cb.setParam(Info()->constBuffers[i], name, v);
				i++;
			}
		}

		struct TexParam
		{
		public:
			using DataType = ShaderDescData::DataType;
			using TexInfo = ShaderDescData::Texture;

			void create(const TexInfo& info) { _info = &info; }

			Texture* getUpdatedTexture();

			StrView name() const { return _info->name; }
			int bindPoint() const { return _info->bindPoint; }
			DataType dataType() const { return _info->dataType; }

			template<class TEX>
			void setTexParam(TEX* tex) {
				if (_info->dataType != RenderDataTypeUtil::get<TEX>()) {
					_tex.reset(nullptr);
					throw SGE_ERROR("invalid texture type");
				}
				_tex = tex;
			}

		protected:
			SPtr<Texture>	_tex;
			const TexInfo* _info = nullptr;
		};

		template<class V>
		void _setTexParam(StrView name, V* texture) {
			if (!texture) { SGE_ASSERT(false); return; }

			for (auto& p : _texParams) 
			{
				if (0 == p.name().compare(name)) {
					p.setTexParam(texture);
					break;
				}
			}
		}

		MaterialPass*			_pass = nullptr;
		ShaderStage*			_shaderStage = nullptr;
		Vector<ConstBuffer, 4>	_constBufs;
		Vector<TexParam, 4>		_texParams;
	};


	struct MaterialPass_VertexStage : public MaterialPass_Stage {
		using Base = MaterialPass_Stage;

		MaterialPass_VertexStage(MaterialPass* pass, ShaderVertexStage* shaderStage)
			: Base(pass, shaderStage)
		{}
	};

	struct MaterialPass_PixelStage : public MaterialPass_Stage {
		using Base = MaterialPass_Stage;
		MaterialPass_PixelStage(MaterialPass* pass, ShaderPixelStage* shaderStage)
			: Base(pass, shaderStage)
		{}
	};

	class MaterialPass : public RefCountBase
	{
	public:
		using Pass			= MaterialPass;
		using VertexStage	= MaterialPass_VertexStage;
		using PixelStage	= MaterialPass_PixelStage;

		virtual ~MaterialPass() = default;

		void bind(RenderContext* ctx, const VertexLayout* vertexLayout) { onBind(ctx, vertexLayout); }

		const ShaderInfo::Pass* info() { return _shaderPass ? _shaderPass->passInfo() : nullptr; }

		friend class Material;
	protected:

		MaterialPass(Material* mat, ShaderPass* SahderPass) : _material(mat), _shaderPass(SahderPass) {}

		Material*	_material	= nullptr;
		ShaderPass* _shaderPass = nullptr;

		virtual void onBind(RenderContext* ctx, const VertexLayout* vertexLayout) = 0;
		

		template<class V> void _setParam(StrView name, const V& v) {
			if (_vertexStage) _vertexStage->_setParam(name, v);
			if (_pixelStage)  _pixelStage->_setParam(name, v);
		}

		template<class V> void _setTexParam(StrView name, const V& v) {
			if (_vertexStage) _vertexStage->_setTexParam(name, v);
			if (_pixelStage)  _pixelStage->_setTexParam(name, v);
		}

		VertexStage*	_vertexStage	= nullptr;
		PixelStage*		_pixelStage		= nullptr;

	};

	class Material : public RefCountBase
	{
		using Pass			= MaterialPass;
		using Stage			= MaterialPass_Stage;
		using VertexStage	= MaterialPass_VertexStage;
		using PixelStage	= MaterialPass_PixelStage;

	public:
		virtual ~Material() = default;

		void setShader(Shader* shader);


		void setParam(StrView name, Texture2D* v)		{ _setTexParam(name, v); }
		void setParam(StrView name, const i32& v)		{ _setParam(name, v); }
		void setParam(StrView name, const u32& v)		{ _setParam(name, v); }

		void setParam(StrView name, const float& v)		{ _setParam(name, v); }

		void setParam(StrView name, const Tuple2i& v)	{ _setParam(name, v); }
		void setParam(StrView name, const Tuple3i& v)	{ _setParam(name, v); }
		void setParam(StrView name, const Tuple4i& v)	{ _setParam(name, v); }

		void setParam(StrView name, const Tuple2f& v)	{ _setParam(name, v); }
		void setParam(StrView name, const Tuple3f& v)	{ _setParam(name, v); }
		void setParam(StrView name, const Tuple4f& v)	{ _setParam(name, v); }
		void setParam(StrView name, const Color4f& v)	{ _setParam(name, v.toTuple()); }
		void setParam(StrView name, const Mat4f& v)		{ _setParam(name, v); }

		Span<UPtr<Pass>>	passes() { return _passes; }

		Pass* getPass(size_t index) 
		{
			if (index >= _passes.size()) 
			{
				SGE_ASSERT(false);
				return nullptr;
			}
			return _passes[index].get();
		}


	protected:
		template<class V> void _setParam(StrView name, const V& v)
		{
			for (auto& pass : _passes)
			{
				if (pass) pass->_setParam(name, v);
			}
		}

		template<class V> void _setTexParam(StrView name, const V& v) 
		{
			for (auto& pass : _passes) 
			{
				if (pass) pass->_setTexParam(name, v);
			}
		}

		Vector<UPtr<Pass>, 1> _passes;
		SPtr<Shader>		_shader;
		virtual void		onSetShader() {};
		virtual UPtr<Pass>	onCreatePass(ShaderPass* shaderPass) = 0;
	};

}