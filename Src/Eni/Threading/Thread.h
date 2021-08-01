#pragma once

#include "EniConfig.h"

#ifdef ENI_THREADING

#include <cstddef>
#include <cassert>
#include <functional>
#include <memory>

#include <Eni/Threading/ThreadPlatform.h>

namespace Eni::Threading {

enum class ThreadPriority{
	Idle,
	Low,
	BelowNormal,
	Normal,
	AboveNormal,
	High,
	Realtime,
	Error
};

class Thread {
public:
	using ID = ThreadID;

	explicit Thread(const char* name, size_t stackSize, ThreadPriority priority, std::function<void()>&& func);

	~Thread();
	Thread() noexcept;
	Thread(const Thread&) = delete;
	Thread(Thread&& other) noexcept;

	Thread& operator = (const Thread&) = delete;
	Thread& operator = (Thread&& other) noexcept;

	void swap(Thread& other);

	bool joinable() const noexcept;
	void join();

	ID getId() const noexcept;

	const char* getName() const;

	ThreadPriority getPriority() const;
	void setPriority(ThreadPriority priority) const;

	void suspend();
	void resume();
private:
	class Impl;
	Thread& moveTask(Thread& other);
	Impl* _impl = nullptr;
};

namespace ThisThread {
	Thread::ID getId();
	ThreadPriority getPriority();
	void setPriority(ThreadPriority priority);
	void sleepForMs(size_t milliseconds);
	void yield();
}

}

#endif //LF_THREADING

