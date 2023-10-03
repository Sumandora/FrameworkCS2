#include <thread>

#include "Interfaces.hpp"

#include "Memory.hpp"

#include "SDK/Schema/Types/FieldData.hpp"
#include "SDK/Schema/Types/BaseClassData.hpp"
#include "SDK/Schema/SchemaClassInfo.hpp"
#include "SDK/Schema/SchemaSystemTypeScope.hpp"

void initializer()
{
	printf("Hello, world, again!\n");

	Interfaces::getInterfaces();

	Memory::Create();

	auto schema = Interfaces::schemaSystem->FindTypeScopeForModule("libclient.so")->FindDeclaredClass("C_BaseEntity");

	for(std::size_t i = 0; i < schema->fieldsCount; i++) {
		FieldData* field = &schema->fields[i];
		printf("%s at %x\n", field->fieldName, field->offset);
	}

	auto walkTree = [](SchemaClassInfo* classInfo) {
		while(classInfo) {
			printf("%s is based in %s\n", classInfo->className, classInfo->moduleName);
			if(!classInfo->baseClass) {
				classInfo = nullptr;
				continue;
			}
			classInfo = &classInfo->baseClass->classInfo;
		}
	};

	for(std::size_t i = 0; i < schema->baseClassCount; i++) {
		walkTree(&schema->baseClass[i].classInfo);
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
