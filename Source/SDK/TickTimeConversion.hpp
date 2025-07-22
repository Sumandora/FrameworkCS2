#pragma once

static constexpr float TICK_INTERVAL = 1.0F / 64.0F;

static constexpr int time_to_ticks(float dt)
{
	// NOLINTNEXTLINE(bugprone-incorrect-roundings)
	return static_cast<int>(0.5F + dt / TICK_INTERVAL);
}

static constexpr float ticks_to_time(int t)
{
	return TICK_INTERVAL * static_cast<float>(t);
}

static constexpr float round_to_ticks(float t)
{
	return TICK_INTERVAL * static_cast<float>(time_to_ticks(t));
}
