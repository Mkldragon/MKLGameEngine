#pragma once

#include "File.h"

namespace sge {

	class FileStream : NonCopyable {
	public:
#if SGE_OS_WINDOWS
		using NativeFd = ::HANDLE;
		static const NativeFd kInvalid() { return INVALID_HANDLE_VALUE; }
#else
		using NativeFd = int;
		static const NativeFd kInvalid() { return -1; }
#endif

		~FileStream() { close(); }

		bool isOpened() const { return _fd != kInvalid; }

		void openRead(StrView filename);
		void openAppend(StrView filename);
		void openWrite(StrView filename, bool truncate);

		void open(StrView filename, FileMode mode, FileAccess access, FileShareMode shareMode);
		void close();
		void flush();

		u64 fileSize();
		void setFileSize(u64 newSize);

		u64 getPos();
		void setPos(u64 pos);
		void setPosFromEnd(u64 pos);

		void readBytes(Span<u8>       data);
		void writeBytes(Span<const u8> data);

		const String& filename() const { return _filename; }

		NativeFd nativeFd() { return _fd; }

	private:
		void _ensure_fd() { if (_fd == kInvalid()) throw SGE_ERROR("invalid file handle"); }

		String _filename;

		NativeFd _fd = kInvalid;
	};

}
