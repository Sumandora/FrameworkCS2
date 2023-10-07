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

#include "SDK/GameClass/CollisionProperty.hpp"
#include "SDK/GameClass/CSPlayerPawn.hpp"
#include "SDK/GameClass/GameSceneNode.hpp"
#include "SDK/GameClass/Transform.hpp"

#include "SDK/Math/Vector.hpp"

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

bool project(const Vector3& world, ImVec2& screen)
{
	const VMatrix & matrix = *Memory::worldToProjectionMatrix;

	const float z = matrix[2][0] * world[0] + matrix[2][1] * world[1] + matrix[2][2] * world[2] + matrix[2][3];
	const float w = matrix[3][0] * world[0] + matrix[3][1] * world[1] + matrix[3][2] * world[2] + matrix[3][3];
	if (z <= 0.0f || w <= 0.0f)
		return false;

	screen = ImGui::GetIO().DisplaySize;
	screen.x *= 0.5f;
	screen.y *= 0.5f;

	screen.x *= 1.0f + (matrix[0][0] * world[0] + matrix[0][1] * world[1] + matrix[0][2] * world[2] + matrix[0][3]) / w;
	screen.y *= 1.0f - (matrix[1][0] * world[0] + matrix[1][1] * world[1] + matrix[1][2] * world[2] + matrix[1][3]) / w;
	return true;
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
}
