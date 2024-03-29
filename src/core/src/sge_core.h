#pragma once

#include <sge_core/base/sge_base.h>
#include <sge_core/base/sge_macro.h>

#include "sge_core/base/UnitTest.h"
#include "sge_core/base/BinDeserializer.h"
#include "sge_core/base/BinSerializer.h"

#include "sge_core/net/Socket.h"
#include "sge_core/net/Poll.h"

#include "sge_core/allocator/LinearAllocator.h"


#include "sge_core/graph/Color.h"
#include "sge_core/graph/ColorUtil.h"

#include <sge_core/math/Math.h>
#include <sge_core/math/Tuple2.h>
#include <sge_core/math/Tuple3.h>
#include <sge_core/math/Tuple4.h>
#include <sge_core/math/Vec2.h>
#include <sge_core/math/Vec3.h>
#include <sge_core/math/Vec4.h>
#include <sge_core/math/Mat4.h>
#include <sge_core/math/Quat4.h>
#include <sge_core/math/MathGeometry.h>
#include <sge_core/math/MathCamera3.h>

#include "sge_core/file/FilePath.h"
#include "sge_core/file/MemMapFile.h"
#include <sge_core/file/Directory.h>
#include "sge_core/app/ProjectSettings.h"

#include "sge_core/pointer/SPtr.h"
#include "sge_core/pointer/ComPtr.h"

#include "sge_core/string/UtfUtil.h"
#include "sge_core/native_ui/NativeUI.h"

#include "sge_core/ECS/TypeReflection.h"