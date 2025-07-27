#include "../GameHook.hpp"

#include "../../../SDK/NetMessages/NetMessagePB.hpp"

#include "../../../Features/Misc/VoteRevealer/VoteRevealer.hpp"

#include "RetAddrSpoofer.hpp"

void Hooks::Game::OnVoteStart::hook_func(void* csgo_hud_vote, NetMessagePB<CCSUsrMsg_VoteStart>* net_message)
{
	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), csgo_hud_vote, net_message);

	vote_revealer->on_vote_start(net_message);
}
