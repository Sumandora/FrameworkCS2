#pragma once

#include "../VirtualMethod.hpp"

class NetworkGameClient {
public:
	void client_side_predict(int stage);

	VIRTUAL_METHOD(28, force_full_update, void, (const char* reason), (this, reason));
};
