#include "Terminal.h"
#include <cstring>

namespace Eni::Terminal {
	Writer out;



	Writer& Writer::operator <<(const char* str) {
		Terminal::write(str, strlen(str));
		return *this;
	}

}
