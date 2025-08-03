#pragma once

#include <concepts>
#include <utility>

template <typename T>
concept Column = requires(T column) {
	{ T::NAME } -> std::convertible_to<const char*>;
	{ std::as_const(column).fill() };
};

struct CSPlayerController;
struct CSPlayerPawn;

template <typename Column>
concept UpdateColoumnWithController = requires(Column column, CSPlayerController* controller) {
	{ column.update(controller) };
};

template <typename Column>
concept UpdateColoumnWithPawn = requires(Column column, CSPlayerPawn* pawn) {
	{ column.update(pawn) }; // `pawn` can be null in case the controller does not have a pawn.
};

template <typename Column>
concept HasHiddenByDefault = requires() {
	{ Column::HIDDEN_BY_DEFAULT } -> std::convertible_to<bool>;
};

template <typename Column>
static constexpr bool is_hidden_by_default()
{
	if constexpr (HasHiddenByDefault<Column>)
		return Column::HIDDEN_BY_DEFAULT;
	return false;
}
