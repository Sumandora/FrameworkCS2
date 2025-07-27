#include "VoteRevealer.hpp"

#include "../../Feature.hpp"

#include "../../Settings/Color.hpp"

#include "../../../SDK/ChatPrintf.hpp"
#include "../../../SDK/Entities/CSPlayerController.hpp"
#include "../../../SDK/Entities/CSPlayerPawnBase.hpp"
#include "../../../SDK/Entities/GameEntitySystem.hpp"
#include "../../../SDK/Entities/BaseEntity.hpp"
#include "../../../SDK/GameClass/GameEvent.hpp"
#include "../../../SDK/GameClass/Localize.hpp"
#include "../../../SDK/NetMessages/NetMessagePB.hpp"

#include "../../../Interfaces.hpp"
#include "../../../Memory.hpp"

#include "../../../Utils/HTMLEscape.hpp"
#include "../../../Utils/Logging.hpp"

#include <cctype>
#include <cstddef>
#include <format>
#include <string>
#include <string_view>

VoteRevealer::VoteRevealer()
	: Feature("Misc", "Vote revealer")
{
}

void VoteRevealer::on_vote_start(NetMessagePB<CCSUsrMsg_VoteStart>* net_message)
{
	if (!enabled.get())
		return;

	if (Memory::local_player && Memory::local_player->team_id() == static_cast<TeamID>(net_message->pb.team()))
		return; // The player can see the vote themselves.

	const char* disp = *reinterpret_cast<char* const*>(&net_message->pb.disp_str());
	const char* details = *reinterpret_cast<char* const*>(&net_message->pb.details_str());
	const char* other_team = *reinterpret_cast<char* const*>(&net_message->pb.other_team_str());

	if (std::string_view{ other_team } == "#SFUI_otherteam_vote_unimplemented") {
		// If they add a new vote type, then this probably helps debugging,
		// however one should note that change level votes for example also show up here, since they are displayed to both teams.
		// I don't filter those as of right now. (mini TODO)
		Logging::debug("Unimplemented vote string: {} - {} - {}", disp, details, other_team);

		/** found this (switches on net_message->pb.vote_type()), it might help:
		char * FUN_012042c0(undefined4 param_1)

		{
		  switch(param_1) {
		  case 0:
			return "kick";
		  case 1:
			return "changelevel";
		  default:
			return (char *)0x0;
		  case 6:
			return "surrender";
		  case 9:
			return "pausematch";
		  case 10:
			return "unpausematch";
		  case 0xb:
			return "loadbackup";
		  case 0xc:
			return "readyformatch";
		  case 0xd:
			return "notreadyformatch";
		  case 0xe:
			return "starttimeout";
		  }
		}
			*/

		return;
	}

	std::string localized = Interfaces::localize->translate(other_team);

	auto* from_controller = static_cast<CSPlayerController*>(GameEntitySystem::the()->getBaseEntity(net_message->pb.player_slot() + 1));

	std::erase_if(localized, [](char c) {
		return !isprint(c);
	});

	// TODO this is hacky, I think Localize interface has functions for this.
	static const std::string_view PLAYER_NAME_REPLACE{ "{s:s1}" };

	const std::size_t pos = localized.find(PLAYER_NAME_REPLACE);
	if (pos != std::string::npos) {
		auto* to_controller = static_cast<CSPlayerController*>(GameEntitySystem::the()->getBaseEntity(net_message->pb.player_slot_target() + 1));

		std::string player_name = to_controller ? to_controller->get_decorated_player_name() : details;
		html_escape(player_name);

		localized.replace(pos, PLAYER_NAME_REPLACE.size(),
			std::format(R"(<font color="{}">{}</font>)", ChatPrintf::to_html_rgba(player_highlight_color.get()), player_name));
	}

	if (from_controller)
		ChatPrintf::print(R"(<font color="{}">{} <font color="{}">(issued by <font color="{}">{}</font>)</font></font>)",
			ChatPrintf::to_html_rgba(announce_color.get()), localized,
			ChatPrintf::to_html_rgba(issued_by_color.get()),
			ChatPrintf::to_html_rgba(player_highlight_color.get()), from_controller->get_decorated_player_name());
	else
		ChatPrintf::print(R"(<font color="{}">{}</font>)",
			ChatPrintf::to_html_rgba(announce_color.get()), localized);
}

void VoteRevealer::event_handler(GameEvent* event)
{
	if (!enabled.get())
		return;

	if (std::string_view{ event->GetName() } != "vote_cast")
		return;

	auto* controller = static_cast<CSPlayerController*>(GameEntitySystem::the()->getBaseEntity(event->get_userid("userid", -2) + 1));
	if (!controller)
		return;

	int vote_option = event->get_int("vote_option", 0);
	if (vote_option != 0 && vote_option != 1) {
		Logging::debug("Received unknown vote option {}", vote_option);
		return; // Game supports up to 5 vote options, but only the first two are used for yes and no respectively.
	}

	std::string player_name = controller->get_decorated_player_name();
	html_escape(player_name);

	const Color& color = vote_option == 0 ? yes_color : no_color;
	const std::string_view text = vote_option == 0 ? "YES" : "NO";

	ChatPrintf::print(R"(<font color="{}"><font color="{}">{}</font> voted <font color="{}">{}</font></font>)",
		ChatPrintf::to_html_rgba(vote_cast_color.get()),
		ChatPrintf::to_html_rgba(player_highlight_color.get()), player_name,
		ChatPrintf::to_html_rgba(color.get()), text);
}
