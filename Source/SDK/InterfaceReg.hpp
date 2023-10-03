#pragma once

struct InterfaceReg {
	void* m_CreateFn;
	const char* m_pName;
	InterfaceReg* m_pNext;
};
