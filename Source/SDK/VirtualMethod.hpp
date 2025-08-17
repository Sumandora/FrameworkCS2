#pragma once

#include "RetAddrSpoofer.hpp"

#include <cstddef>

// Inspired by danielkrupinski/Osiris

namespace VirtualMethod {
	inline void** get_vtable(const void* game_class)
	{
		return *reinterpret_cast<void** const*>(game_class);
	}

	template <typename Ret, std::size_t Index, typename... Args>
	inline Ret invoke(const void* game_class, Args... args)
	{
		return RetAddrSpoofer::invoke<Ret, decltype(game_class), Args...>(get_vtable(game_class)[Index], game_class, args...);
	}

	template <typename Ret, std::size_t Index, typename... Args>
	inline Ret invoke_static(const void* game_class, Args... args)
	{
		return RetAddrSpoofer::invoke<Ret, Args...>(get_vtable(game_class)[Index], args...);
	}
}

#define VIRTUAL_METHOD(index, name, returnType, argsType, argsCall) \
	inline returnType name argsType                                 \
	{                                                               \
		return VirtualMethod::invoke<returnType, index> argsCall;   \
	}                                                               \
	static constexpr std::size_t name##_index = index

// In case a virtual function doesn't take thisptr as first argument, this can be used
#define STATIC_VIRTUAL_METHOD(index, name, returnType, argsType, argsCall) \
	inline returnType name argsType                                        \
	{                                                                      \
		return VirtualMethod::invoke_static<returnType, index> argsCall;   \
	}                                                                      \
	static constexpr std::size_t name##_index = index
