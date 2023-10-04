#include <thread>

#include "Interfaces.hpp"

#include "Memory.hpp"

#include "SDK/Schema/Types/FieldData.hpp"
#include "SDK/Schema/Types/SchemaType.hpp"
#include "SDK/Schema/Types/StaticFieldData.hpp"
#include "SDK/Schema/SchemaClassInfo.hpp"
#include "SDK/Schema/SchemaSystemTypeScope.hpp"

void initializer()
{
	printf("Hello, world, again!\n");

	Interfaces::getInterfaces();

	Memory::Create();

	auto schema = Interfaces::schemaSystem->FindTypeScopeForModule("libclient.so")->FindDeclaredClass("CBodyComponentSkeletonInstance");

	for(std::size_t i = 0; i < schema->fieldsCount; i++) {
		FieldData* field = &schema->fields[i];
		printf("%s at %x with type %s\n", field->fieldName, field->offset, field->type->name);
	}

	for(std::size_t i = 0; i < schema->staticFieldsCount; i++) {
		StaticFieldData* field = &schema->staticFields[i];
		printf("%s is type of %s and is located at %p\n", field->fieldName, field->type->name, field->data);
	}
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
