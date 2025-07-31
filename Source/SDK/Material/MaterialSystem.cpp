#include "MaterialSystem.hpp"

#include "../GameClass/KeyValues3.hpp"

#include <cstring>

Material** MaterialSystem::create_material(const char* name, const char* kv_text_buffer)
{
	static const KV3ID GENERIC_KV{
		.name = "generic",
		.magic1 = 0x469806E97412167C,
		.magic2 = 0xE73790B53EE6F2AF,
	};

	// Credits: https://www.unknowncheats.me/forum/3912703-post15.html
	auto* kv_buffer = new unsigned char[0x100 + sizeof(KeyValues3)];
	memset(kv_buffer, 0, 0x100 + sizeof(KeyValues3));

	auto* kv = reinterpret_cast<KeyValues3*>(kv_buffer + 0x100);
	kv->load_kv3(kv_text_buffer, GENERIC_KV);

	Material** le_mat = nullptr;

	create_material(&le_mat, name, kv, 0, 1);

	return le_mat;
}
