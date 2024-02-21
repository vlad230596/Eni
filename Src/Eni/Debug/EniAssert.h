#pragma once

#include "Debug.h"
#include "Eni/Terminal/Terminal.h"

namespace Eni::Debug {
    class Assert : public LogWriter {
	public:
		~Assert(){
			for(;;);
		}
	};


#define eniAssert(expression) \
	do { \
		if(!(expression)) { \
			Eni::Debug::Assert() \
				<< "Assert: " <<  #expression << Eni::Terminal::NewLine \
				<< "File: " << __FILE__ << Eni::Terminal::NewLine; \
		} \
	}while(false)

}
