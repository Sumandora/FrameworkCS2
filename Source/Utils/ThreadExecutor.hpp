#pragma once

#include <functional>
#include <mutex>
#include <vector>

class ThreadExecutor {
private:
	std::vector<std::function<void()>> queued_tasks;
	std::mutex queued_tasks_lock;

public:
	void queue(std::function<void()>);
	void run_all_queued_functions();
};
