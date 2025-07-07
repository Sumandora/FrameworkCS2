#include "MaterialSystem.hpp"

#include "RetAddrSpoofer.hpp"

#include "../VirtualMethod.hpp"

#include "../GameClass/KeyValues3.hpp"

#include <cstring>

void MaterialSystem::create_material(Material*** out_material, const char* material_name, KeyValues3* key_values, std::uint32_t unk1, std::uint8_t unk2)
{
	// No `this` pointer, WTF???
	RetAddrSpoofer::invoke<void>(VirtualMethod::get_vtable(this)[28], out_material, nullptr, material_name, key_values, unk1, unk2);
}

Material* MaterialSystem::create_material(const char* name, const char* kv_text_buffer)
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

	return *le_mat;
}
