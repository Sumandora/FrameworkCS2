#include "ThreadExecutor.hpp"

#include <functional>
#include <mutex>
#include <utility>

void ThreadExecutor::queue(std::function<void()> action)
{
	const std::lock_guard g{ queued_tasks_lock };
	queued_tasks.emplace_back(std::move(action));
}

void ThreadExecutor::run_all_queued_functions()
{

	const std::lock_guard g{ queued_tasks_lock };
	std::erase_if(queued_tasks, [](const std::function<void()>& action) {
		action();
		return true;
	});
}
