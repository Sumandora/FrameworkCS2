#include "Materials.hpp"

#include "../Utils/Logging.hpp"

#include <array>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

static constexpr std::string_view MATERIALS_SUBDIR = "Materials";

static std::filesystem::path materials_dir;
static bool available = false;

static std::vector<Serialization::Materials::Material> materials;

static constexpr std::format_string<int> UNLIT_GENERIC
	= R"(<!-- kv3 encoding:text:version{{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}} format:generic:version{{7412167c-06e9-4698-aff2-e63eb59037e7}} -->
{{
	shader = "csgo_unlitgeneric.vfx"

	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
	F_BLEND_MODE = 1
	F_DISABLE_Z_BUFFERING = {}

	g_vColorTint = [1, 1, 1, 1]

	TextureAmbientOcclusion = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tAmbientOcclusion = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tTintMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
}})";

static constexpr std::format_string<int> GLOWY
	= R"(<!-- kv3 encoding:text:version{{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}} format:generic:version{{7412167c-06e9-4698-aff2-e63eb59037e7}} -->
{{
	shader = "csgo_complex.vfx"
	
	F_SELF_ILLUM = 1
	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
	F_DISABLE_Z_BUFFERING = {}

	g_vColorTint = [ 1.000000, 1.000000, 1.000000, 1.000000 ]
	g_flSelfIllumScale = [ 3.000000, 3.000000, 3.000000, 3.000000 ]
	g_flSelfIllumBrightness = [ 3.000000, 3.000000, 3.000000, 3.000000 ]
	g_vSelfIllumTint = [ 10.000000, 10.000000, 10.000000, 10.000000 ]

	g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tSelfIllumMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	TextureAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
	g_tAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
}})";

static constexpr std::array DEFAULT_MATERIALS = {
	std::pair<std::string, std::format_string<int>>{ "Unlit generic", UNLIT_GENERIC },
	std::pair<std::string, std::format_string<int>>{ "Glowy", GLOWY },
};

static void fill_materials_with_defaults()
{
	for (const auto& [material, kv] : DEFAULT_MATERIALS) {
		for (const bool zbuffering : { true, false }) {
			materials.emplace_back(
				material + (zbuffering ? "" : " (No Z-Buffering)"),
				[kv, zbuffering] { return std::format(kv, zbuffering ? 0 : 1); });
		}
	}
}

void Serialization::Materials::initialize_directory(const std::filesystem::path& config_dir)
{
	materials_dir = config_dir / MATERIALS_SUBDIR;

	const bool is_oobe = !std::filesystem::exists(materials_dir);

	if (is_oobe) {
		std::error_code status;
		if (!std::filesystem::create_directory(materials_dir, status)) {
			Logging::error("Failed to create materials directory, materials are unavailable: {}", status.message());
			fill_materials_with_defaults();
			available = false;
			return;
		}

		for (const auto& [material, kv] : DEFAULT_MATERIALS) {
			for (const bool zbuffering : { true, false }) {
				std::ofstream out{ materials_dir / material += zbuffering ? ".kv3" : " (No Z-Buffering).kv3" };
				out << std::format(kv, zbuffering ? 0 : 1);
				out.close();
			}
		}
	}

	available = true;
}

const std::vector<Serialization::Materials::Material>& Serialization::Materials::get_materials()
{
	if (!available) {
		return materials;
	}

	// Don't update it every time, that would probably cause hard drive failure in the long term.
	using std::chrono::system_clock;
	static constexpr auto REFRESH_INTERVAL = std::chrono::seconds(5);
	static system_clock::time_point last_update = system_clock::from_time_t(0);
	const system_clock::time_point right_now = system_clock::now();
	if (right_now - last_update > REFRESH_INTERVAL) {
		materials.clear();
		for (const std::filesystem::path& path : std::filesystem::directory_iterator{ materials_dir }) {
			if (path.extension() != ".kv3")
				continue;

			materials.emplace_back(path.stem(), [path] -> std::string {
				std::ifstream f{ path };
				if (!f) {
					return {};
				}
				std::string s{ std::istreambuf_iterator<char>{ f }, {} };
				f.close();
				return s;
			});
		}
		last_update = right_now;
	}

	return materials;
}
