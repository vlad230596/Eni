#include "Terminal.h"
#include <cstring>

namespace Eni::Terminal {
	Writer out;



	Writer& Writer::operator <<(const char* str) {
#if defined(ENI_TERMINAL)
		Terminal::write(str, strlen(str));
#endif
		return *this;
	}

}
