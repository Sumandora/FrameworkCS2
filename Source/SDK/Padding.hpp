#pragma once

#include <cstddef>

#define CONCAT(a, b) a##b
#define PADDING_NAME(line) CONCAT(PAD, line)

#define PADDING(size) [[maybe_unused]] std::byte PADDING_NAME(__LINE__)[size]{}
