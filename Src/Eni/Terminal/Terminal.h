#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

#include <EniConfig.h>

namespace Eni::Terminal {
	static const char* NewLine = "\r\n";

	void write(const char* msg, std::size_t length);


	struct Writer {
		template<typename T>
		Writer& operator <<(const T& value){
#if defined(ENI_TERMINAL)
			auto str = std::to_string(value);
			Terminal::write(str.data(), str.length());
#endif
			return *this;
		}

		Writer& operator <<(const char* str);
	};

	extern Writer out;
}
