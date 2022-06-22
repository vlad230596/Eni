#pragma once

#include <Eni/Terminal/Terminal.h>

namespace Eni::Debug {
	class LogWriter {
	public:
		LogWriter(){

		}
		template<typename T>
		LogWriter& operator << (const T& value) {
			Terminal::out << value;
			return *this;
		}

		~LogWriter() {
			Terminal::out << Terminal::NewLine;
			Terminal::flush();
		}
	};

	#define eniDebug() Eni::Debug::LogWriter()
}
