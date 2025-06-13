#pragma once

#include "NodeType.hpp"

#include "imgui.h"

#include "magic_enum/magic_enum.hpp"

#include <cstddef>
#include <type_traits>
#include <utility>
#include <variant>

class AnyEnum {
	std::size_t underlying;

public:
	template <typename E>
		requires std::is_enum_v<E> && (sizeof(E) < sizeof(underlying))
	explicit AnyEnum(E e)
		: underlying(std::to_underlying(e))
	{
	}

	template <typename E>
		requires std::is_enum_v<E> && (sizeof(E) < sizeof(underlying))
	E get() const
	{
		return magic_enum::enum_cast<E>(underlying).value();
	}
};

class NodeResult {
	std::variant<std::monostate, float, bool, ImColor, AnyEnum> variant;
	static_assert(magic_enum::enum_count<NodeType>() - 1 == std::variant_size_v<decltype(variant)>);

public:
	/**
	 * Note for the uninformed:
	 * There are several usages of the default constructor, basically NodeResult{}, or returning {}
	 * This constructor constructs a variant with the first type default initialized, basically creating an "empty" NodeResult
	 */

	template <typename... Ts>
	// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
	NodeResult(Ts&&... args)
		: variant(std::forward<Ts>(args)...)
	{
	}

	template <typename E>
		requires std::is_enum_v<E>
	// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
	NodeResult(E e)
		: variant(AnyEnum(e))
	{
	}

	[[nodiscard]] bool empty() const
	{
		return std::holds_alternative<std::monostate>(variant);
	}

	[[nodiscard]] bool full() const
	{
		return !std::holds_alternative<std::monostate>(variant);
	}

	template <typename T>
	T get() const
	{
		if constexpr (std::is_enum_v<T>)
			return std::get<AnyEnum>(variant).get<T>();
		else
			return std::get<T>(variant);
	}
};
