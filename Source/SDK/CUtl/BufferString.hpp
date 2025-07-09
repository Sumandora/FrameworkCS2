#pragma once

struct BufferString {
	static BufferString just(const char* str);

	~BufferString();

	void purge(int unk1);
	void insert(int unk1, const char* str, int unk2, bool unk3);
	
	int length;
	int unk1;
	char* str;
};
