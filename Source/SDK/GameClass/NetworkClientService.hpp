#pragma once

#include "../Padding.hpp"

class NetworkGameClient;

class NetworkClientService {
	PADDING(0xA0);

public:
	NetworkGameClient* network_game_client;
};
