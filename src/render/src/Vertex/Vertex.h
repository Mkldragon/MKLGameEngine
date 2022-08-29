#pragma once

#include "../RenderDataType.h"

namespace sge {




	enum class VertexType : u64 { None };

	struct VertexTypeUtil {
		static constexpr VertexType addPos(VertexType t, RenderDataType posType) {
			return static_cast<VertexType>(static_cast<u64>(t)
				| static_cast<u64>(posType));
		}

		static constexpr VertexType addColor(VertexType t, RenderDataType colorType, u8 colorCount) {
			return static_cast<VertexType>(static_cast<u64>(t)
				| (static_cast<u64>(colorType) << 8)
				| (static_cast<u64>(colorCount) << 16));
		}

		static constexpr VertexType addUv(VertexType t, RenderDataType uvType, u8 uvCount) {
			return static_cast<VertexType>(static_cast<u64>(t)
				| (static_cast<u64>(uvType) << 18)
				| (static_cast<u64>(uvCount) << 26));
		}

		static constexpr VertexType addNormal(VertexType t, RenderDataType normalType, u8 normalCount) {
			return static_cast<VertexType>(static_cast<u64>(t)
				| (static_cast<u64>(normalType) << 32)
				| (static_cast<u64>(normalCount) << 40));
		}

		static constexpr VertexType addTangent(VertexType t, u8 tangentCount) {
			return static_cast<VertexType>(static_cast<u64>(t)
				| (static_cast<u64>(tangentCount) << 42));
		}
		static constexpr VertexType addBinormal(VertexType t, u8 binormalCount) {
			return static_cast<VertexType>(static_cast<u64>(t)
				| (static_cast<u64>(binormalCount) << 44));
		}
		static constexpr VertexType addVertexId(VertexType t) {
			return static_cast<VertexType>(static_cast<u64>(t)
				| (static_cast<u64>(1) << 47));
		}


		static constexpr VertexType make(
			RenderDataType posType,
			RenderDataType colorType, u8 colorCount,
			RenderDataType uvType, u8 uvCount,
			RenderDataType normalType, u8 normalCount, u8 tangentCount, u8 binormalCount)
		{
			VertexType t = addPos(VertexType::None, posType);
			if (colorCount)	 t = addColor(t, colorType, colorCount);
			if (uvCount)	 t = addUv(t, uvType, uvCount);
			if (normalCount) {
				t = addNormal(t, normalType, normalCount);
				t = addTangent(t, tangentCount);
				t = addBinormal(t, binormalCount);
			}
			return t;
		}
	};




	enum class VertexSemantic : u16;

SGE_ENUM_ALL_OPERATOR(VertexSemantic)

	enum class VertexSemanticType : u8 {
		None,
		POSITION,
		COLOR,
		TEXCOORD,
		NORMAL,
		TANGENT,
		BINORMAL,
		SV_VertexID,
	};


	#define VertexSemanticType_ENUM_LIST(E) \
		E(None)		\
		E(POSITION)	\
		E(COLOR)	\
		E(TEXCOORD)	\
		E(NORMAL)	\
		E(TANGENT)	\
		E(BINORMAL)	\
		E(SV_VertexID,) \
	//----
	SGE_ENUM_STR_UTIL(VertexSemanticType)


	struct Vertex_SemanticUtil 
	{
		using Semantic = VertexSemantic;
		using Type = VertexSemanticType;
		using Index = u8;

		static constexpr Semantic _make(Type type, Index index) {
			return static_cast<Semantic>((enumInt(type) << 8) | index);
		};

		static constexpr u16 make(Type type, Index index) {
			return static_cast<u16>((enumInt(type) << 8) | index);
		};
		static constexpr Type	getType(Semantic v) { return static_cast<Type>(enumInt(v) >> 8); }
		static constexpr Index	getIndex(Semantic v) { return static_cast<u8>(enumInt(v)); }
	};


	enum class VertexSemantic : u16 {
		None = 0,
		Pos = Vertex_SemanticUtil::make(VertexSemanticType::POSITION, 0),

		Color0 = Vertex_SemanticUtil::make(VertexSemanticType::COLOR, 0),
		Color1 = Vertex_SemanticUtil::make(VertexSemanticType::COLOR, 1),
		Color2 = Vertex_SemanticUtil::make(VertexSemanticType::COLOR, 2),
		Color3 = Vertex_SemanticUtil::make(VertexSemanticType::COLOR, 3),

		TexCoord0 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 0),
		TexCoord1 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 1),
		TexCoord2 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 2),
		TexCoord3 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 3),
		TexCoord4 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 4),
		TexCoord5 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 5),
		TexCoord6 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 6),
		TexCoord7 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 7),

		TexCoord8 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 8),
		TexCoord9 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 9),
		TexCoord10 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 10),
		TexCoord11 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 11),
		TexCoord12 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 12),
		TexCoord13 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 13),
		TexCoord14 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 14),
		TexCoord15 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 15),

		TexCoord16 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 16),
		TexCoord17 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 17),
		TexCoord18 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 18),
		TexCoord19 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 19),
		TexCoord20 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 20),
		TexCoord21 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 21),
		TexCoord22 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 22),
		TexCoord23 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 23),

		TexCoord24 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 24),
		TexCoord25 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 25),
		TexCoord26 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 26),
		TexCoord27 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 27),
		TexCoord28 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 28),
		TexCoord29 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 29),
		TexCoord30 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 30),
		TexCoord31 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 31),
		TexCoord32 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 32),
		TexCoord33 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 33),
		TexCoord34 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 34),
		TexCoord35 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 35),
		TexCoord36 = Vertex_SemanticUtil::make(VertexSemanticType::TEXCOORD, 36),

		Normal = Vertex_SemanticUtil::make(VertexSemanticType::NORMAL, 0),
		Tangent = Vertex_SemanticUtil::make(VertexSemanticType::TANGENT, 0),
		Binormal = Vertex_SemanticUtil::make(VertexSemanticType::BINORMAL, 0),

		SV_VertexID = Vertex_SemanticUtil::make(VertexSemanticType::SV_VertexID, 0),
	};

#define VertexSemantic_ENUM_LIST(E) \
	E(None)	\
	E(Pos) \
	\
	E(Color0) \
	E(Color1) \
	E(Color2) \
	E(Color3) \
	\
	E(TexCoord0) \
	E(TexCoord1) \
	E(TexCoord2) \
	E(TexCoord3) \
	E(TexCoord4) \
	E(TexCoord5) \
	E(TexCoord6) \
	E(TexCoord7) \
	\
	E(TexCoord8) \
	E(TexCoord9) \
	E(TexCoord10) \
	E(TexCoord11) \
	E(TexCoord12) \
	E(TexCoord13) \
	E(TexCoord14) \
	E(TexCoord15) \
	\
	E(TexCoord16) \
	E(TexCoord17) \
	E(TexCoord18) \
	E(TexCoord19) \
	E(TexCoord20) \
	E(TexCoord21) \
	E(TexCoord22) \
	E(TexCoord23) \
	\
	E(TexCoord24) \
	E(TexCoord25) \
	E(TexCoord26) \
	E(TexCoord27) \
	E(TexCoord28) \
	E(TexCoord29) \
	E(TexCoord30) \
	E(TexCoord31) \
	E(TexCoord32) \
	E(TexCoord33) \
	E(TexCoord34) \
	E(TexCoord35) \
	E(TexCoord36) \
	\
	E(Normal)   \
	E(Tangent)  \
	E(Binormal) \
	\
	E(SV_VertexID) \
//----
	SGE_ENUM_STR_UTIL(VertexSemantic)

	struct VertexLayout : public NonCopyable
	{

		using Semantic = VertexSemantic;
		using DataType = RenderDataType;

		struct Element {
			Semantic	semantic = Semantic::None;
			u16			offset = 0;
			DataType	dataType = DataType::None;
		};

		VertexType type = VertexType::None;
		size_t	stride = 0;
		Vector<Element, 16>	elements;

		template<class VERTEX, class ATTR>
		void addElement(Semantic semantic, ATTR VERTEX::* attr, size_t index = 0) 
		{
			if (std::is_array<ATTR>()) {
				size_t n = std::extent<ATTR>();
				for (size_t i = 0; i < n; i++) {
					_addElement(semantic + static_cast<int>(i), attr, i);
				}
			}
			else {
				_addElement(semantic, attr, 0);
			}
		}

		const Element* find(Semantic semantic) const 
		{
			for (auto& e : elements) 
			{
				if (e.semantic == semantic) return &e;
			}
			return nullptr;
		}
	private:
		template<class VERTEX, class ATTR>
		void _addElement(Semantic semantic, ATTR VERTEX::* attr, size_t index) 
		{
			auto& o = elements.push_back();
			o.semantic = semantic;
			using A = std::remove_extent<ATTR>::type;
			o.dataType = RenderDataTypeUtil::get<A>();
			o.offset = static_cast<u16>(memberOffset(attr) + sizeof(A) * index);
		}
	};


	struct VertexBase {
		using Semantic		= VertexSemantic;
		using PosType		= void;
		using ColorType		= void;
		using UvType		= void;
		using NormalType	= void;

		static const RenderDataType	kPosType = RenderDataType::None;
		static const RenderDataType	kColorType = RenderDataType::None;
		static const RenderDataType	kUvType = RenderDataType::None;
		static const RenderDataType	kNormalType = RenderDataType::None;

		static const u8 kColorCount = 0;
		static const u8 kUvCount = 0;
		static const u8 kNormalCount = 0;
		static const u8 kTangentCount = 0;
		static const u8 kBinormalCount = 0;

		static const VertexType kType = VertexType::None;
	};


	template<class POS_TYPE>
	struct VertexT_Pos : public VertexBase
	{
		using PosType = POS_TYPE;
		POS_TYPE pos;

		static const RenderDataType	kPosType = RenderDataTypeUtil::get<POS_TYPE>();
		static const VertexType kType = VertexTypeUtil::addPos(VertexType::None, kPosType);

		static const VertexLayout* layout() {
			static const VertexLayout* s = VertexLayoutManager::instance()->getLayout(kType);
			return s;
		}

		static void onRegister(VertexLayout* layout) {
			layout->addElement(Semantic::Pos, &VertexT_Pos::pos);
		}
	};

	template<class COLOR_TYPE, u8 COLOR_COUNT, class BASE>
	struct VertexT_Color : public BASE
	{
		using ColorType = COLOR_TYPE;
		COLOR_TYPE	color[COLOR_COUNT];

		static const RenderDataType	kColorType = RenderDataTypeUtil::get<COLOR_TYPE>();
		static const u8 kColorCount = COLOR_COUNT;
		static const VertexType kType = VertexTypeUtil::addColor(BASE::kType, kColorType, kColorCount);

		static const VertexLayout* layout() {
			static const VertexLayout* s = VertexLayoutManager::instance()->getLayout(kType);
			return s;
		}

		static void onRegister(VertexLayout* layout) {
			BASE::onRegister(layout);
			layout->addElement(Semantic::Color0, &VertexT_Color::color);
		}
	};

	template<class UV_TYPE, u8 UV_COUNT, class BASE>
	struct VertexT_Uv : public BASE
	{
		using UvType = UV_TYPE;
		UV_TYPE		uv[UV_COUNT];

		static const RenderDataType	kUvType = RenderDataTypeUtil::get<UV_TYPE>();
		static const u8 kUvCount = UV_COUNT;
		static const VertexType kType = VertexTypeUtil::addUv(BASE::kType, kUvType, kUvCount);

		static const VertexLayout* layout() {
			static const VertexLayout* s = VertexLayoutManager::instance()->getLayout(kType);
			return s;
		}

		static void onRegister(VertexLayout* layout) {
			BASE::onRegister(layout);
			layout->addElement(Semantic::TexCoord0, &VertexT_Uv::uv);
		}
	};

	template<class NORMAL_TYPE, u8 NORMAL_COUNT, class BASE>
	struct VertexT_Normal : public BASE
	{
		using NormalType = NORMAL_TYPE;
		NORMAL_TYPE	normal[NORMAL_COUNT];

		static const RenderDataType	kNormalType = RenderDataTypeUtil::get<NORMAL_TYPE>();
		static const u8 kNormalCount = NORMAL_COUNT;
		static const VertexType kType = VertexTypeUtil::addNormal(BASE::kType, kNormalType, kNormalCount);

		static const VertexLayout* layout() {
			static const VertexLayout* s = VertexLayoutManager::instance()->getLayout(kType);
			return s;
		}

		static void onRegister(VertexLayout* layout) {
			BASE::onRegister(layout);
			layout->addElement(Semantic::Normal, &VertexT_Normal::normal);
		}
	};

	template<class TANGENT_TYPE, u8 TANGENT_COUNT, class BASE>
	struct VertexT_Tangent : public BASE
	{
		using TangentType = TANGENT_TYPE;
		TANGENT_TYPE	tangent[TANGENT_COUNT];

		static const RenderDataType	kTangentType = RenderDataTypeUtil::get<TANGENT_TYPE>();
		static const u8 kTangentCount = TANGENT_COUNT;
		static const VertexType kType = VertexTypeUtil::addTangent(BASE::kType, kTangentCount);

		static const VertexLayout* layout() {
			static const VertexLayout* s = VertexLayoutManager::instance()->getLayout(kType);
			return s;
		}

		static void onRegister(VertexLayout* layout) {
			static_assert(std::is_same<TangentType, NormalType>::value, "");

			BASE::onRegister(layout);
			layout->addElement(Semantic::Tangent, &VertexT_Tangent::tangent);
		}
	};

	template<class BINORMAL_TYPE, u8 BINORMAL_COUNT, class BASE>
	struct VertexT_Binormal : public BASE
	{
		using BinormalType = BINORMAL_TYPE;
		BINORMAL_TYPE	binormal[BINORMAL_COUNT];

		static const RenderDataType	kBinormalType = RenderDataTypeUtil::get<BINORMAL_TYPE>();
		static const u8 kBinormalCount = BINORMAL_COUNT;
		static const VertexType kType = VertexTypeUtil::addBinormal(BASE::kType, kBinormalCount);

		static const VertexLayout* layout() {
			static const VertexLayout* s = VertexLayoutManager::instance()->getLayout(kType);
			return s;
		}

		static void onRegister(VertexLayout* layout) {
			static_assert(std::is_same<BinormalType, NormalType>::value, "");

			BASE::onRegister(layout);
			layout->addElement(Semantic::Binormal, &VertexT_Binormal::binormal);
		}
	};

	template<class BASE>
	struct VertexT_VertexId : public BASE {
		u32 vertexId;

		static const VertexType kType = VertexTypeUtil::addVertexId(BASE::kType);

		static const VertexLayout* s_layout() {
			static const VertexLayout* s = VertexLayoutManager::instance()->getLayout(kType);
			return s;
		}

		static void onRegister(VertexLayout* layout) {
			layout->addElement(Semantic::SV_VertexID, &VertexT_VertexId::vertexId);
		}
	};

	using Vertex_Pos2f			= VertexT_Pos<Tuple2f>;
	using Vertex_Pos			= VertexT_Pos<Tuple3f>;

	using Vertex_PosNormal		= VertexT_Normal<Tuple3f, 1, Vertex_Pos>;

	using Vertex_PosColor		= VertexT_Color<Color4b, 1, Vertex_Pos>;
	using Vertex_PosColorNormal = VertexT_Normal<Tuple3f, 1, Vertex_PosColor>;


	template<u8 UV_COUNT> using Vertex_PosUv				= VertexT_Uv<Tuple2f, UV_COUNT, Vertex_Pos>;
	template<u8 UV_COUNT> using Vertex_PosColorUv			= VertexT_Uv<Tuple2f, UV_COUNT, Vertex_PosColor>;

	template<u8 UV_COUNT> using Vertex_PosNormalUv			= VertexT_Normal<Tuple3f, 1, Vertex_PosUv<UV_COUNT>>;
	template<u8 UV_COUNT> using Vertex_PosColorNormalUv		= VertexT_Normal<Tuple3f, 1, Vertex_PosColorUv<UV_COUNT>>;

	template<u8 UV_COUNT> using Vertex_PosTangentUv			= VertexT_Tangent<Tuple3f, 1, Vertex_PosNormalUv<UV_COUNT>>;
	template<u8 UV_COUNT> using Vertex_PosColorTangentUv	= VertexT_Tangent<Tuple3f, 1, Vertex_PosColorNormalUv<UV_COUNT>>;

	template<u8 UV_COUNT> using Vertex_PosBinormalUv		= VertexT_Binormal<Tuple3f, 1, Vertex_PosTangentUv<UV_COUNT>>;
	template<u8 UV_COUNT> using Vertex_PosColorBinormalUv	= VertexT_Binormal<Tuple3f, 1, Vertex_PosColorTangentUv<UV_COUNT>>;

	using Vertex_VertexId = VertexT_VertexId<VertexBase>;
}