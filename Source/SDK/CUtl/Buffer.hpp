#pragma once

#include "Memory.hpp"

#include "Byteswap.hpp"

struct UtlBuffer {
	using UtlBufferOverflowFunc = bool (UtlBuffer::*)(int size);

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

	explicit UtlBuffer(int grow_size = 0, int init_size = 0, int flags = 0);

	void ensure_capacity(int size);
};

static_assert(sizeof(UtlBuffer) == 0x50);
