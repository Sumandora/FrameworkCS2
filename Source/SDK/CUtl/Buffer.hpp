#pragma once

#include "Memory.hpp"

#include "Byteswap.hpp"

#include "../Padding.hpp"

using BufferFlags = int;

struct UtlBuffer {
	using UtlBufferOverflowFunc = bool (UtlBuffer::*)(int size);

	PADDING(0x8); // vtable probably?

	CUtlMemory<unsigned char> memory;
	int get;
	int put;

	unsigned char error;
	unsigned char flags;
	unsigned char reserved;

	int tab;
	int max_put;
	int offset;

	UtlBufferOverflowFunc get_overflow_func;
	UtlBufferOverflowFunc put_overflow_func;

	Byteswap byteswap;

	explicit UtlBuffer(int grow_size = 0, int init_size = 0, BufferFlags flags = 0);
	~UtlBuffer();

	void ensure_capacity(int size);
};

static_assert(sizeof(UtlBuffer) == 0x58);
