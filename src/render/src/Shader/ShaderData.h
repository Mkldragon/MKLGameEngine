#pragma once

#include "sge_core.h"
#include "RenderDataType.h"
#include "Vertex/Vertex.h"
#include "sge_core/serializer/json/JsonUtil.h"

namespace sge
{

	using Json = nlohmann::json;

	enum class ShaderDescMask {
		None,
		Vertex = 1 << 0,
		Pixel = 1 << 1,
	};



	enum class ShaderPropType {
		None,
		Int,
		Float,
		Vec2f,
		Vec3f,
		Vec4f,
		Color4f,
	};

	// RenderStateEnum
#pragma region RenderStateEnum

	enum class BlendOP
	{
		Add,
		Sub,
		RevSub,
		Min,
		Max,
	};
	#define BlendOP_ENUM_LIST(E) \
	E(Add) \
	E(Sub) \
	E(RevSub) \
	E(Min) \
	E(Max) \
	//----
	SGE_ENUM_STR_UTIL(BlendOP)


	enum class BlendFactor
	{
		One,
		Zero,
		SrcColor,
		SrcAlpha,
		DstColor,
		DstAlpha,
		OneMinusSrcColor,
		OneMinusSrcAlpha,
		OneMinusDstColor,
		OneMinusDstAlpha,
	};

	#define BlendFactor_ENUM_LIST(E) \
	E(One) \
	E(Zero) \
	E(SrcColor) \
	E(SrcAlpha) \
	E(DstColor) \
	E(DstAlpha) \
	E(OneMinusSrcColor) \
	E(OneMinusSrcAlpha) \
	E(OneMinusDstColor) \
	E(OneMinusDstAlpha) \
	//----
	SGE_ENUM_STR_UTIL(BlendFactor)


	enum class ShaderCull
	{
		Back,
		Front,
		Off,
	};

	#define ShaderCull_ENUM_LIST(E) \
	E(Back) \
	E(Front) \
	E(Off) \
	//----
	SGE_ENUM_STR_UTIL(ShaderCull)

	enum class DepthTest
	{
		None,
		Less,
		Greater,
		LEqual,
		GEqual,
		Equal,
		NotEqual,
		Always
	};
	#define DepthTest_ENUM_LIST(E) \
	E(None) \
	E(Less) \
	E(Greater) \
	E(LEqual) \
	E(GEqual) \
	E(Equal) \
	E(NotEqual) \
	E(Always) \
	//----
	SGE_ENUM_STR_UTIL(DepthTest)

#pragma endregion

	

	inline
		StrView enumStr(ShaderPropType v)
	{
		switch (v)
		{
#define E(V) case ShaderPropType::V: return #V;
			E(None)
				E(Int)
				E(Float)
				E(Vec2f)
				E(Vec3f)
				E(Vec4f)
				E(Color4f);
#undef E
		default: return "";
		}
	}


	struct  RenderState
	{
	public:

		struct  Blend
		{
			String op	= "Add";
			String src	= "One";
			String dest = "Zero";

			template<class SE>
			void onJson(SE& se) {
				SGE_NAMED_IO(se, op);
				SGE_NAMED_IO(se, src);
				SGE_NAMED_IO(se, dest);
			}
		};

		String cull = "Off";
		String wireFrame = "false";

		Blend blendRGB;
		Blend blendAlpha;

		String depthTest = "LEqual";
		String depthWrite = "Off";
		
		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, cull);
			SGE_NAMED_IO(se, wireFrame);
			SGE_NAMED_IO(se, blendRGB);
			SGE_NAMED_IO(se, blendAlpha);

			SGE_NAMED_IO(se, depthTest);
			SGE_NAMED_IO(se, depthWrite);
		}

	};


	struct ShaderInfo
	{

		struct Pass
		{
		public:

			String name;
			String vsEntryPt;
			String psEntryPt;
			RenderState renderState;

			template<class SE>
			void onJson(SE& se) {
				SGE_NAMED_IO(se, name);
				SGE_NAMED_IO(se, vsEntryPt);
				SGE_NAMED_IO(se, psEntryPt); 
				SGE_NAMED_IO(se, renderState);
			}
		};

		struct  Prop
		{
		public:
			ShaderPropType propTypr = ShaderPropType::None;
			String		name;
			String		displayName;
			String		defaultValue;

			template<class SE>
			void onJson(SE& se) {
				SGE_NAMED_IO(se, name);
				SGE_NAMED_IO(se, displayName);
				SGE_NAMED_IO(se, defaultValue);
			}
		};

	public:
		String path;
		String fileName;
		String shaderName;
		Vector<Prop, 8> props;
		Vector<Pass, 2> passes;

		void clear();

		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, path);
			SGE_NAMED_IO(se, fileName);
			SGE_NAMED_IO(se, shaderName);
			SGE_NAMED_IO(se, props);
			SGE_NAMED_IO(se, passes);
		}
	};



	enum class TokenType
	{
		None,
		Identifier,
		Operator,
		Number,
		String,
		Comment,
	};

	struct Token
	{
	public:
		TokenType type;
		String value;

		bool CheckToken(TokenType _type, String _expectValue)	{ return (type == _type && value == _expectValue); }
		bool CheckToken(TokenType _type)						{ return (type == _type); }
		bool CheckValue(String _expectValue)					{ return value == _expectValue; }
	};







	class  ShaderDescData
	{
	public:
		using DataType = RenderDataType;

		void loadFile(StrView _filename)
		{
			filename = _filename;
			JsonUtil::readFile(_filename, *this);
		}

		String	filename;
		String	profile;
		ShaderDescMask mask = ShaderDescMask::None;

		class  ShaderInputParam
		{
		public:
			String			name;
			VertexSemantic	semantic = VertexSemantic::None;
			RenderDataType	dataType = RenderDataType::None;

			template<class SE>
			void onJson(SE& se) {
				SGE_NAMED_IO(se, name);
				SGE_NAMED_IO(se, semantic);
				SGE_NAMED_IO(se, dataType);
			}
		};

		class  ShaderVariable
		{
		public:
			String			name;
			int				offset = 0;
			RenderDataType	dataType = RenderDataType::None;

			template<class SE>
			void onJson(SE& se) {
				SGE_NAMED_IO(se, name);
				SGE_NAMED_IO(se, offset);
				SGE_NAMED_IO(se, dataType);
			}

		};
				
		class  ConstBufferInfo
		{
		public:
			String	name;
			int		bindPoint = 0;
			int		bindCount = 0;
			size_t	dataSize = 0;
			Vector<ShaderVariable, 4> variables;

			const ShaderVariable* findVariable(StrView propname) const
			{
				for (auto& v : variables)
				{
					if (v.name == propname) return &v;
				}

				return nullptr;
			}

			template<class SE>
			void onJson(SE& se) {
				SGE_NAMED_IO(se, name);
				SGE_NAMED_IO(se, bindPoint);
				SGE_NAMED_IO(se, bindCount);
				SGE_NAMED_IO(se, dataSize);
				SGE_NAMED_IO(se, variables);
			}

		};

		Vector<ShaderInputParam, 8> inputs;
		Vector<ConstBufferInfo, 8> constBuffers;

		//-------Texture---------
		class Texture
		{
		public:
			String		name;
			i16			bindPoint = 0;
			i16			bindCount = 0;
			DataType	dataType = DataType::None;

			template<class SE>
			void onJson(SE& se) {
				SGE_NAMED_IO(se, name);
				SGE_NAMED_IO(se, bindPoint);
				SGE_NAMED_IO(se, bindCount);
				SGE_NAMED_IO(se, dataType);
			}
		};
		Vector<Texture, 8>		textures;
		class Sampler {
		public:
			String		name;
			i16			bindPoint = 0;
			i16			bindCount = 0;
			RenderDataType	dataType = RenderDataType::None;

			template<class SE>
			void onJson(SE& se) {
				SGE_NAMED_IO(se, name);
				SGE_NAMED_IO(se, bindPoint);
				SGE_NAMED_IO(se, bindCount);
				SGE_NAMED_IO(se, dataType);
			}
		};
		Vector<Sampler, 8>		samplers;

		//---------Texture---------


		void Clear();

		template<class SE>
		void onJson(SE& se) {
			SGE_NAMED_IO(se, profile);
			SGE_NAMED_IO(se, inputs);
			SGE_NAMED_IO(se, constBuffers);
			SGE_NAMED_IO(se, textures);
			SGE_NAMED_IO(se, samplers);
		}
	};


	inline
	bool enumTryParse(ShaderPropType& outValue, StrView str) 
	{
#define E(V) if (str == #V) { outValue = ShaderPropType::V; return true; }
		E(None)
			E(Int)
			E(Float)
			E(Vec2f)
			E(Vec3f)
			E(Vec4f)
			E(Color4f)
#undef E
			return false;
	}


}