#pragma once

#include "../Padding.hpp"

class NetworkGameClient;

class NetworkClientService {
	PADDING(0xB8);

public:
	NetworkGameClient* network_game_client;
};
