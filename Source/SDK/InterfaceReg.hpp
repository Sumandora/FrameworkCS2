#ifndef SDK_INTERFACEREG
#define SDK_INTERFACEREG

struct InterfaceReg {
	void* m_CreateFn;
	const char* m_pName;
	InterfaceReg* m_pNext;
};

#endif
