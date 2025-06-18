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
};

static_assert(sizeof(UtlBuffer) == 80);
