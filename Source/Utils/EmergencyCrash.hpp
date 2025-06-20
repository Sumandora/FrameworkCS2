#pragma once

#include "Logging.hpp" // IWYU pragma: keep

#include <csignal> // IWYU pragma: keep

/**
 * Emergency crashes are basically asserts, but much more powerful, only use them when the
 * condition is vital. They are even active in release mode and they terminate the entire game
 * with no crash handling. This is useful when you have a feature fail in a miserable way that
 * cannot be recovered from and is likely to require developer intervention to be fixed.
 */

// NOLINTBEGIN(cppcoreguidelines-avoid-do-while)
#define EMERGENCYCRASH_IF(cond, message)                                     \
	do {                                                                     \
		if (cond) {                                                          \
			Logging::error("EMERGENCY CRASH! " #message);                    \
			if (!raise(SIGKILL))                                             \
				exit(0); /* status = 0, so that no crash handlers kick in */ \
		}                                                                    \
	} while (0)
// NOLINTEND(cppcoreguidelines-avoid-do-while)
