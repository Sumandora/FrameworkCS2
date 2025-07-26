#pragma once

#include "../Padding.hpp"

struct BaseEntity;

struct Source2ClientPrediction {
	OFFSET(bool, in_prediction, 0x34);
	OFFSET(BaseEntity*, prediction_entity, 0x48);
};
