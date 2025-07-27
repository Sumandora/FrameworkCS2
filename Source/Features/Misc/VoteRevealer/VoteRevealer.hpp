#pragma once

#include "../../Feature.hpp"

#include "../../Settings/Checkbox.hpp"
#include "../../Settings/Color.hpp"

#include "../../../Utils/UninitializedObject.hpp"

#include "../../../SDK/NetMessages/NetMessagePB.hpp"

#include "ColorPreview.hpp"

#include "cstrike15_usermessages.pb.h"

#include "imgui.h"

struct GameEvent;

class VoteRevealer : public Feature {
	Checkbox enabled{ this, "Enabled", false };

	// TODO This is very overwhelming... (Preview maybe?)
	Color announce_color{ this, "Announce color", ImColor{ 1.0F, 0.0F, 0.0F, 1.0F } };
	Color issued_by_color{ this, "Issued-by color", ImColor{ 1.0F, 1.0F, 1.0F, 1.0F } };
	Color vote_cast_color{ this, "Vote color", ImColor{ 1.0F, 1.0F, 1.0F, 1.0F } };
	Color yes_color{ this, "Yes color", ImColor{ 0.0F, 1.0F, 0.0F, 1.0F } };
	Color no_color{ this, "No color", ImColor{ 1.0F, 0.0F, 0.0F, 1.0F } };
	Color player_highlight_color{ this, "Player highlight color", ImColor{ 1.0F, 1.0F, 1.0F, 1.0F } };

	ColorPreview colors{ this, "Preview",
		announce_color, issued_by_color, vote_cast_color,
		yes_color, no_color, player_highlight_color };

public:
	VoteRevealer();

	void on_vote_start(NetMessagePB<CCSUsrMsg_VoteStart>* net_message);
	void event_handler(GameEvent* event);
};

inline UninitializedObject<VoteRevealer> vote_revealer;
