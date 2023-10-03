#include <thread>

#include "Interfaces.hpp"

void initializer()
{
	printf("Hello, world, again!\n");
	Interfaces::getInterfaces();
}

int __attribute((constructor)) startup()
{
	printf("Hello, world!\n");
	std::thread t(initializer);
	t.detach();

	return 0;
}

void __attribute((destructor)) shutdown()
{
}
