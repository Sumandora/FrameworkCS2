#include "VTexDecoder.hpp"

#include "../GUI/TextureManager.hpp"

#include "../SDK/Padding.hpp"

#include "Logging.hpp"

#include "magic_enum/magic_enum.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "lz4.h"

using RawImage = GUI::TextureManager::RawImage;

enum class VTexFlags : std::uint16_t {
	SUGGEST_CLAMPS = 1 << 0,
	SUGGEST_CLAMPT = 1 << 1,
	SUGGEST_CLAMPU = 1 << 2,
	NO_LOD = 1 << 3,
	CUBE_TEXTURE = 1 << 4,
	VOLUME_TEXTURE = 1 << 5,
	TEXTURE_ARRAY = 1 << 6,
	PANORAMA_DILATE_COLOR = 1 << 7,
	PANORAMA_CONVERT_TO_YCOCG_DXT5 = 1 << 8,
	CREATE_LINEAR_API_TEXTURE = 1 << 9,
};

enum class VTexFormat : std::uint8_t {
	UNKNOWN = 0,
	DXT1 = 1,
	DXT5 = 2,
	I8 = 3,
	RGBA8888 = 4,
	R16 = 5,
	RG1616 = 6,
	RGBA16161616 = 7,
	R16F = 8,
	RG1616F = 9,
	RGBA16161616F = 10,
	R32F = 11,
	RG3232F = 12,
	RGB323232F = 13,
	RGBA32323232F = 14,
	JPEG_RGBA8888 = 15,
	PNG_RGBA8888 = 16,
	JPEG_DXT5 = 17,
	PNG_DXT5 = 18,
	BC6H = 19,
	BC7 = 20,
	ATI2N = 21,
	IA88 = 22,
	ETC2 = 23,
	ETC2_EAC = 24,
	R11_EAC = 25,
	RG11_EAC = 26,
	ATI1N = 27,
	BGRA8888 = 28,
	WEBP_RGBA8888 = 29,
	WEBP_DXT5 = 30,
};

// NOLINTNEXTLINE(performance-enum-size)
enum class VTexExtraDataType : std::uint32_t {
	UNKNOWN = 0,
	FALLBACK_BITS = 1,
	SHEET = 2,
	METADATA = 3,
	COMPRESSED_MIP_SIZE = 4,
	CUBEMAP_RADIANCE_SH = 5,
};

struct VTexHeader {
	std::int32_t header_length;
	PADDING(4); // This is the version, maybe I should print it, TODO
	std::int32_t resource_offset;
	std::int32_t resource_count;

	[[nodiscard]] std::uint32_t resource_index() const
	{
		return resource_offset + 8;
	}
};

struct VTexResource {
	char type[4];
	std::int32_t resource_offset;
	std::int32_t resource_length;

	[[nodiscard]] std::uint32_t resource_index() const
	{
		return resource_offset + 4;
	}
};

struct VTexExtraData {
	VTexExtraDataType extra_data_type;
	std::uint32_t extra_data_offset;
	std::uint32_t extra_data_length;
};
static_assert(sizeof(VTexExtraData) == 12);

struct VTexData {
	std::uint16_t version;
	std::uint16_t flags;
	PADDING(16); // reflectivity
	std::uint16_t width;
	std::uint16_t height;
	std::uint16_t depth;

	VTexFormat image_format;
	std::uint8_t mip_map_count;

	PADDING(4);

	std::uint32_t extra_data_offset;
	std::uint32_t extra_data_count;

	PADDING(8 + 4 + 4);

	PADDING(4); // bump map scale

	[[nodiscard]] std::uint32_t image_length(int mipmap_width, int mipmap_height) const
	{
		switch (image_format) {
			using enum VTexFormat;
		case RGBA8888:
		case BGRA8888:
			return mipmap_width * mipmap_height * 4; // 4 bytes per pixel
		default:
			Logging::error("Unknown image format: {}", magic_enum::enum_name(image_format));
			return 0; // Pretty good guess...
		}
		std::unreachable();
	}
};

static bool decode_buffer(std::vector<char>& buffer, VTexFormat image_format, int width, int height)
{
	switch (image_format) {
		using enum VTexFormat;
	case BGRA8888: {
		auto index = 0;
		for (auto y = 0; y < height; y++)
			for (auto x = 0; x < width; x++) {
				std::swap(buffer[index], buffer[index + 2]);
				index += 4;
			}

		return true;
	}
	case PNG_RGBA8888:
	case RGBA8888:
		return true;
	default:
		Logging::error("Unknown image format: {}", magic_enum::enum_name(image_format));
		return false;
	}
}

std::optional<RawImage> VTexDecoder::decode(std::vector<char> input_buffer)
{
	std::istringstream stream(std::string{ input_buffer.begin(), input_buffer.end() }, std::ios_base::binary);
	VTexHeader header;
	stream.read(reinterpret_cast<char*>(&header), sizeof(header));

	stream.seekg(header.resource_offset + 8, std::ios_base::beg);

	VTexResource data;
	std::int64_t data_offset = 0;
	bool found = false;

	for (int i = 0; i < header.resource_count; i++) {
		data_offset = stream.tellg();
		stream.read(reinterpret_cast<char*>(&data), sizeof(VTexResource));
		if (std::memcmp(data.type, "DATA", 4) == 0) {
			found = true;
			break;
		}
	}

	if (!found)
		return std::nullopt;

	data_offset += data.resource_offset + 4;
	stream.seekg(data_offset, std::ios_base::beg);

	VTexData vtex_data;
	stream.read(reinterpret_cast<char*>(&vtex_data), sizeof(VTexData));

	std::vector<std::uint32_t> compressed_mips;

	stream.seekg(data_offset + vtex_data.extra_data_offset + 32, std::ios_base::beg);
	for (std::uint32_t extra_data_index = 0; extra_data_index < vtex_data.extra_data_count; extra_data_index++) {
		VTexExtraData vtex_extra_data;
		const std::int64_t extra_data_offset = stream.tellg();
		stream.read(reinterpret_cast<char*>(&vtex_extra_data), sizeof(VTexExtraData));

		if (vtex_extra_data.extra_data_type == VTexExtraDataType::COMPRESSED_MIP_SIZE) {
			const std::int64_t prev_pos = stream.tellg();
			stream.seekg(extra_data_offset + vtex_extra_data.extra_data_offset + 4, std::ios_base::beg);

			stream.ignore(8);

			std::int32_t mips_count = 0;
			stream.read(reinterpret_cast<char*>(&mips_count), sizeof(mips_count));

			compressed_mips.resize(mips_count);
			for (int i = 0; i < mips_count; i++) {
				stream.read(reinterpret_cast<char*>(&compressed_mips[i]), sizeof(std::uint32_t));
			}
			stream.seekg(prev_pos);
		}
	}

	auto mipmap_width = vtex_data.width >> (vtex_data.mip_map_count - 1);
	auto mipmap_height = vtex_data.height >> (vtex_data.mip_map_count - 1);

	std::size_t compressed_mips_index = 0;

	stream.seekg(header.header_length, std::ios_base::beg);
	for (std::uint8_t mip_index = 0; mip_index < vtex_data.mip_map_count; mip_index++) {

		const std::uint32_t size = vtex_data.image_length(mipmap_width, mipmap_height);
		if (size == 0)
			return std::nullopt;

		std::uint32_t compressed_mip_size = size;
		if (compressed_mips_index < compressed_mips.size())
			compressed_mip_size = compressed_mips[compressed_mips.size() - compressed_mips_index - 1];

		compressed_mips_index++;

		if (mip_index != vtex_data.mip_map_count - 1) {
			stream.ignore(compressed_mip_size);
			// NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
			goto next_elem;
		}

		{
			std::vector<char> buffer;
			buffer.resize(size);

			if (size == compressed_mip_size) {
				stream.read(buffer.data(), size);
			} else {
				std::vector<char> compressed_buffer(compressed_mip_size);
				stream.read(compressed_buffer.data(), compressed_mip_size);
				LZ4_decompress_safe(compressed_buffer.data(), buffer.data(),
					static_cast<int>(compressed_buffer.size()), static_cast<int>(buffer.size()));
			}

			if (!decode_buffer(buffer, vtex_data.image_format, mipmap_width, mipmap_height))
				return std::nullopt;
			// TODO I know this function should be able to do the color conversion for me, but I can't get it to work...

			// static auto* convert_image_format
			// 	= BCRL::signature(
			// 		Memory::mem_mgr,
			// 		SignatureScanner::PatternSignature::for_array_of_bytes<"55 49 89 D2 48 89 E5 41 57 49 89 FF">(),
			// 		BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("librendersystemvulkan.so"))
			// 		  .expect<bool (*)(const unsigned char* src, int srcImageFormat,
			// 			  unsigned char* dst, int dstImageFormat,
			// 			  int width, int height, int srcStride, int dstStride)>("Couldn't find ImageLoader::ConvertImageFormat");

			// std::vector<char> dest;
			// dest.resize(buffer.size());

			// convert_image_format((unsigned char*)buffer.data(), 28,
			// 	(unsigned char*)dest.data(), 4,
			// 	mipmapWidth, mipmapHeight, 0, 0);

			// buffer = dest;

			// Pray that these bytes somehow make up an image... (Not all formats are like this, but I couldn't care less about the others...)

			std::vector<RawImage::Color> rgba(static_cast<std::size_t>(mipmap_width) * static_cast<std::size_t>(mipmap_height));

			// TODO don't copy manually
			int i = 0;
			for (int y = 0; y < mipmap_height; y++) {
				for (int x = 0; x < mipmap_width; x++) {
					rgba[y * mipmap_width + x] = RawImage::Color{
						.r = static_cast<uint8_t>(buffer[i + 0]),
						.g = static_cast<uint8_t>(buffer[i + 1]),
						.b = static_cast<uint8_t>(buffer[i + 2]),
						.a = static_cast<uint8_t>(buffer[i + 3]),
					};
					i += 4;
				}
			}

			return RawImage(mipmap_width, mipmap_height, rgba);
		}

	next_elem:
		mipmap_width *= 2;
		mipmap_height *= 2;
	}

	return std::nullopt;
}
