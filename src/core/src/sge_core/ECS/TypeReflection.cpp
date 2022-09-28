#include "TypeReflection.h"


namespace sge
{


	//-----

	SGE_TYPE_IMPL(float)
	SGE_TYPE_IMPL(double)

	SGE_TYPE_IMPL(int8_t)
	SGE_TYPE_IMPL(int16_t)
	SGE_TYPE_IMPL(int32_t)
	SGE_TYPE_IMPL(int64_t)

	SGE_TYPE_IMPL(uint8_t)
	SGE_TYPE_IMPL(uint16_t)
	SGE_TYPE_IMPL(uint32_t)
	SGE_TYPE_IMPL(uint64_t)

	SGE_TYPE_IMPL(char16_t)
	SGE_TYPE_IMPL(char32_t)
	SGE_TYPE_IMPL(wchar_t)


	//SGE_TypeOf_impl(Object)

	template<> const TypeInfo* typeOf<Object>() {
		class TI : public TypeInfo {
		public:
			TI() {
				name = "Object";
			}
		};
		static TI ti;
		return &ti;
	}

	TypeManager* TypeManager::instance() {
		static TypeManager m;
		return &m;
	}

}
