#include <thread>

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

void printFields(SchemaClassInfo* classInfo)
{
	printf("Regular fields:\n");
	for (std::size_t i = 0; i < classInfo->fieldsCount; i++) {
		FieldData* field = &classInfo->fields[i];
		printf("%s at %x with type %s\n", field->fieldName, field->offset, field->type->name);
	}

	printf("Static fields:\n");
	for (std::size_t i = 0; i < classInfo->staticFieldsCount; i++) {
		StaticFieldData* field = &classInfo->staticFields[i];
		printf("%s is type of %s and is located at %p\n", field->fieldName, field->type->name, field->data);
	}
}

void initializer()
{
	printf("Hello, world, again!\n");

	Interfaces::getInterfaces();

	Memory::Create();

	printFields(CSPlayerPawn::classInfo());

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
