#pragma once

#include "RetAddrSpoofer.hpp"
#include <cstdint>

// Inspired by danielkrupinski/Osiris

namespace VirtualMethod {
	inline void** getVTable(void* gameClass)
	{
		return *reinterpret_cast<void***>(gameClass);
	}

	template <typename Ret, unsigned long Index, typename... Args>
	inline Ret invoke(void* gameClass, Args... args)
	{
		return RetAddrSpoofer::invoke<Ret, void*, Args...>(getVTable(gameClass)[Index], gameClass, args...);
	}
}

#define VIRTUAL_METHOD(index, name, returnType, argsType, argsCall) \
	inline returnType name argsType                                 \
	{                                                               \
		return VirtualMethod::invoke<returnType, index> argsCall;   \
	}                                                               \
	static constexpr std::size_t name##_index = index
