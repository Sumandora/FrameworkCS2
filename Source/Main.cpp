#include <thread>

#include "ExecutableMalloc/MemoryManagerAllocator.hpp"
#include "Interfaces.hpp"

#include "Memory.hpp"

#include "GraphicsHook/GraphicsHook.hpp"

#include "imgui.h"
#include "SDK/Schema/SchemaClassInfo.hpp"
#include "SDK/Schema/SchemaSystem.hpp"
#include "SDK/Schema/SchemaSystemTypeScope.hpp"
#include "SDK/Schema/Types/FieldData.hpp"
#include "SDK/Schema/Types/SchemaType.hpp"
#include "SDK/Schema/Types/StaticFieldData.hpp"

#include "SDK/Entities/BaseEntity.hpp"
#include "SDK/Entities/GameEntitySystem.hpp"

#include "SDK/Entities/CSPlayerPawn.hpp"
#include "SDK/GameClass/CollisionProperty.hpp"
#include "SDK/GameClass/GameSceneNode.hpp"
#include "SDK/GameClass/Transform.hpp"

#include "SDK/Math/Vector.hpp"
#include "GameHook/GameHook.hpp"

static void initializer()
{
	printf("Hello, world, again!\n");

	Memory::mem_mgr.update();

	Interfaces::getInterfaces();

	Memory::Create();

	if (!GraphicsHook::hookSDL()) {
		printf("Failed to hook SDL\n");
		return;
	}

	if (!GraphicsHook::hookVulkan()) {
		printf("Failed to hook vulkan\n");
		return;
	}

	GameHook::hook();
}

int __attribute((constructor, used)) startup()
{
	printf("Hello, world!\n");
	std::thread t(initializer);
	t.detach();

	return 0;
}

void __attribute((destructor)) shutdown()
{
	GraphicsHook::unhookSDL();
	GraphicsHook::unhookVulkan();
	GameHook::unhook();
}
