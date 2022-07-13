#pragma once

namespace sge {

	struct FilePath {
		static String getDir(StrView path);
		static StrView basename(StrView path);
		static StrView extension(StrView path);

		static bool		isRealpath(const StrView& path);
		static String	realpath(StrView path);
	};

}